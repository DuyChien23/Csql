//
// Created by chiendd on 21/07/2024.
//

#include "database.h"

#include "entity.h"
#include "../util/errors.h"
#include "../util/helpers.h"
#include "../view/query_result_view.h"
#include "../view/table_view.h"
#include "page/b_plus_node/leaf_b_plus_node.h"

void Database::createTable(std::ostream &anOutput, SharedEntityPtr &anEntity) {
    validateTableNotExisted(anEntity->getName());
    validateCreateTable(anEntity);

    mEntity[anEntity->getName()] = anEntity;

    anEntity->addIndexingMetadata(anEntity->getClusteredKey(), true);

    anEntity->eachAttribute([&](Attribute *anAttribute) {
        if ((anAttribute->isPrimary() || anAttribute->isUnique()) && anAttribute->getName() != anEntity->
            getClusteredKey()) {
            anEntity->addIndexingMetadata(anAttribute->getName(), false);
        }
    });

    // make entity file
    std::ofstream f(createEntityPath(anEntity->getName()));
    f.close();

    anEntity->save();
    anEntity->encode(getFstream(anEntity->getName()));

    QueryResultView(0, "create table").show(anOutput);
}

void Database::validateCreateTable(const SharedEntityPtr &anEntityPtr) {
    Attribute *primaryAttribute = nullptr;
    Attribute *uniqueAttribute = nullptr;
    bool valid = true;
    anEntityPtr->eachAttribute([&](Attribute *anAtribute) {
        if (anAtribute->isPrimary()) {
            if (!primaryAttribute) {
                primaryAttribute = anAtribute;
            } else {
                throw Errors("Too many primary keys");
            }
        }

        if (anAtribute->isUnique() && !uniqueAttribute) {
            uniqueAttribute = anAtribute;
        }

        if (anAtribute->getReference()) {
            bool flag = false;

            eachEntity([&](SharedEntityPtr anEntity) {
                if (anEntity->getAttribute(anAtribute->getReference()->getAttributeName()) &&
                    anEntity->getName() == anAtribute->getReference()->getEntityName()) {
                    flag = true;
                }
            });

            valid &= flag;
        }
    });

    if (!valid) {
        throw Errors("Table reference not found");
    }

    if (primaryAttribute) uniqueAttribute = primaryAttribute;

    if (!uniqueAttribute) {
        uniqueAttribute = (new Attribute())->setName(SpecialKey::HIDDEN_KEY)->setType(DataTypes::int_type)->
                setPrimary(true);
        anEntityPtr->addAttribute(uniqueAttribute);
    }
    anEntityPtr->setClusteredKey(uniqueAttribute->getName());
}

void Database::showTables(std::ostream &anOutput) {
    TableView theTableView("Tables in database: " + this->name);
    eachEntity([&](SharedEntityPtr anEntity) {
        theTableView.addData(anEntity->getName());
    });
    theTableView.show(anOutput);
}

void Database::describeTable(std::ostream &anOutput, std::string aEntityName) {
    validateTableExisted(aEntityName);

    TableView theTableView("Field", "Type", "Null", "Key", "Extra");
    getEntity(aEntityName)->eachAttribute([&](Attribute *anAttribute) {
        theTableView.addData(
            anAttribute->getName(),
            Helpers::SqlTypeHandle::covertDataTypeToString(anAttribute->getType()),
            anAttribute->isNull() ? "YES" : "NO",
            anAttribute->isPrimary() ? "PRI" : anAttribute->isUnique() ? "UNI" : "",
            anAttribute->getAutoIncrement() ? "auto_increment" : "");
    });
    theTableView.show(anOutput);
}

void Database::dropTable(std::ostream &anOutput, std::string aEntityName) {
    validateTableExisted(aEntityName);

    deleteEntityFile(aEntityName);
    QueryResultView(0, "drop table").show(anOutput);
}

void Database::insert(
    std::ostream &anOutput,
    std::string aEntityName,
    std::vector<std::pair<std::string, std::string> > &aValues,
    bool hasAtrributeName) {
    validateTableExisted(aEntityName);
    SharedEntityPtr theEntity = getEntity(aEntityName);
    Tuple theTuple;

    for (int i = 0; i < aValues.size(); i++) {
        Attribute *theAttribute;
        if (hasAtrributeName) {
            theAttribute = theEntity.get()->getAttribute(aValues[i].first);
        } else {
            theAttribute = theEntity->getAttributes().at(i);
        }
        if (theAttribute == nullptr) {
            throw Errors("Column not found");
        }
        theTuple[theAttribute->getName()] = Helpers::SqlTypeHandle::covertStringToSqlType(
            theAttribute->getType(), aValues[i].second);
    }

    theEntity->eachAttribute([&](Attribute *anAttribute) {
        if (!theTuple.contains(anAttribute->getName()) && !anAttribute->isNull()) {
            if (anAttribute->isAutoIncrement()) {
                theTuple[anAttribute->getName()] = SqlIntType(anAttribute->getAutoIncrement());
            } else {
                throw Errors("Key " + anAttribute->getName() + " not null");
            }
        }
    });

    doInsert(anOutput, aEntityName, theTuple);
}

void Database::doInsert(std::ostream &anOutput, std::string aEntityName, Tuple &theTuple) {
    SharedEntityPtr theEntity = getEntity(aEntityName);

    try {
        validateInsert(anOutput, theEntity, theTuple);
    } catch (Errors e) {
        QueryResultView(0, e.what()).show(anOutput);
        return;
    }

    //=====================INSERT=========================================
    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
            Helpers::TupleHandle::addBNodeKey(theTuple, indexingMetadata);
            setBTree(indexingMetadata, theTuple);
        } else {
            setBTree(indexingMetadata,
                     Helpers::TupleHandle::makeSecondaryTuple(theTuple, indexingMetadata,
                                                              theEntity->getClusteredKey()));
        }
    });

    saveEntity(theEntity);

    QueryResultView(1, "inserted tuple").show(anOutput);
}

void Database::validateInsert(std::ostream &anOutput, const SharedEntityPtr &theEntity, const Tuple &theTuple) {
    theEntity->eachAttribute([&](Attribute *anAttribute) {
        if (!theTuple.contains(anAttribute->getName())) {
            if (!anAttribute->isNull()) {
                throw Errors("Key" + anAttribute->getName() + " not null");
            }
        }
    });

    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool _) {
        auto key = Helpers::TupleHandle::genBNodeKey(theTuple, indexingMetadata);
        auto searchPage = searchBtree(indexingMetadata, key);
        if (searchPage != endLeaf() && Helpers::TupleHandle::getBTreeKey(*searchPage) == key) {
            throw Errors("Duplicate key");
        }
    });

    //=====================CHECK-FOREIGN-KEY============================
    for (auto &attribute: theEntity->getAttributes()) {
        if (attribute->getReference()) {
            auto referrenceEntityName = attribute->getReference()->getEntityName();
            auto referrenceAttributeName = attribute->getReference()->getAttributeName();
            auto referrenceEntity = getEntity(attribute->getReference()->getEntityName());

            referrenceEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool _) {
                if (indexingMetadata.keys[0] == referrenceAttributeName && indexingMetadata.keys.size() == 1) {
                    auto key = Helpers::TupleHandle::genBNodeKey(theTuple, indexingMetadata);
                    auto searchPage = searchBtree(indexingMetadata, key);
                    if (searchPage != endLeaf() && Helpers::TupleHandle::getBTreeKey(*searchPage) == key) {
                        throw Errors("Referrence not found");
                    }
                }
            });
        }
    }
}

void Database::select(std::ostream &anOutput, SQLQueryPtr &aSelectQuery) {
    std::string theEntityName = aSelectQuery->getEntityName();
    validateTableExisted(theEntityName);

    SharedEntityPtr theEntity = getEntity(theEntityName);

    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
            auto iter = beginLeaf(indexingMetadata);
            while (iter != endLeaf()) {
                for (auto e: (*iter)) {
                    anOutput << e.first << ": " << Helpers::SqlTypeHandle::covertSqlTypeToString(e.second) << "\t|";
                }
                nextLeaf(iter);
            }
        }
    });

    //=====================TARGET==================================================
    if (aSelectQuery->isSelectedAll) {
        auto addTargetFromEntity = [&](std::string entityName) {
            validateTableExisted(entityName);
            getEntity(entityName)->eachAttribute([&](Attribute *anAttribute) {
                aSelectQuery->addTaget(entityName, anAttribute->getName());
            });
        };
        addTargetFromEntity(theEntityName);
        for (auto &joinExpression: aSelectQuery->getListJoin()) {
            addTargetFromEntity(joinExpression.targetEntityName);
        }
    }

    //=================WHERE===========================================================

    //=================TABLE-VIEW=======================================================
    TableView theTableView(aSelectQuery->headers);

    theTableView.show(anOutput);
}

void Database::deleteTuples(std::ostream &anOutput, const SQLQueryPtr &aDeleteQuery) {
    // TODO:
}

// make sure the table is existed
void Database::validateTableExisted(std::string aEntityName) {
    if (!getEntity(std::move(aEntityName))) {
        throw Errors("Table does not exist");
    }
}

// make sure the table isn't existed
void Database::validateTableNotExisted(std::string aEntityName) {
    if (getEntity(std::move(aEntityName))) {
        throw Errors("Table already exists");
    }
}
