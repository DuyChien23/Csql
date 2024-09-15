//
// Created by chiendd on 21/07/2024.
//

#include "database.h"

#include "entity.h"
#include "../util/errors.h"
#include "../util/helpers.h"
#include "../view/query_result_view.h"
#include "../view/table_view.h"

namespace Csql {
    void Database::createTable(std::ostream &anOutput, SharedEntityPtr &anEntity) {
        validateTableNotExisted(anEntity->getName());
        validateCreateTable(anEntity);

        mEntity[anEntity->getName()] = anEntity;

        // make entity file
        std::ofstream f(createEntityPath(anEntity->getName()));
        f.close();

        anEntity->save();
        anEntity->encode(getFstream(anEntity->getName()));

        QueryResultView(0, "create table").show(anOutput);
    }

    void Database::validateCreateTable(const SharedEntityPtr &anEntityPtr) {
        int cntPrimaryKey = 0;
        bool valid = true;
        anEntityPtr->eachAttribute([&](Attribute *anAtribute) {
            if (anAtribute->isPrimary()) {
                cntPrimaryKey ++;
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

        if (cntPrimaryKey > 1) {
            throw Errors("Too many primary keys");
        }
        if (!valid) {
            throw Errors("Table reference not found");
        }
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

        TableView theTableView("Field", "Type", "Null", "Key", "Default", "Extra");
        getEntity(aEntityName)->eachAttribute([&](Attribute* anAttribute) {
           theTableView.addData(
               anAttribute->getName(),
               "anAttribute->getType()",
               anAttribute->isNull() ? "YES" : "NO",
               anAttribute->isPrimary() ? "PRI" : "",
               "",
               anAttribute->getAutoIncrement() ? "auto_increment" : "");
        });
        theTableView.show(anOutput);
    }

    void Database::dropTable(std::ostream &anOutput, std::string aEntityName) {
        validateTableExisted(aEntityName);

        deleteEntityFile(aEntityName);
        QueryResultView(0, "drop table").show(anOutput);
    }

    void Database::insert(std::ostream &anOutput, std::string aEntityName, const Tuple &theTuple) {
        validateTableExisted(aEntityName);

        SharedEntityPtr theEntity = getEntity(aEntityName);

        if (!validateInsert(anOutput, theEntity, theTuple)) {
            return;
        }

        //=====================INSERT=========================================
        bool inserted = false;
        eachDataPage(aEntityName, [&](SharedPagePtr& aPage) {
            if (inserted) return;
            if (aPage->addTuple(theTuple)) {
                inserted = true;
                writePage(aPage);
            }
        });
        if (!inserted) {
            SharedPagePtr freePage = popFreePage(aEntityName);
            freePage->addTuple(theTuple);
            pushDataPage(aEntityName, freePage);
            writePage(freePage);
        }

        saveEntity(theEntity);

        QueryResultView(1, "inserted tuple").show(anOutput);
    }

    bool Database::validateInsert(std::ostream &anOutput, const SharedEntityPtr& theEntity, const Tuple &theTuple) {
        theEntity->eachAttribute([&](Attribute* anAttribute) {
            if (!theTuple.contains(anAttribute->getName())) {
                if (!anAttribute->isNull()) {
                    throw Errors("Key" + anAttribute->getName() + " not null");
                }
            }
        });

        //=====================CHECK-FOREIGN-KEY============================
        for (auto &attribute : *theEntity->getAttributes()) {
            if (attribute->getReference()) {
                bool foundReference = false;

                eachTuple(attribute->getReference()->getEntityName(), [&](Tuple& aTuple) -> bool {
                    if (theTuple.at(attribute->getName()) == aTuple[attribute->getReference()->getAttributeName()]) {
                        foundReference = true;
                    }
                });

                if (!foundReference) {
                    QueryResultView(0, "Not found reference for column " + attribute->getName(), false).show(anOutput);
                    return false;
                }
            }
        }

        //=====================CHECK-PRIMARY-UNIQUE-KEY======================
        bool dulicated = false;
        eachTuple(theEntity->getName(), [&](Tuple& aTuple) -> bool {
            for (auto &attribute : *theEntity->getAttributes()) {
                if (attribute->isUnique() && theTuple.at(attribute->getName()) == aTuple[attribute->getName()]) {
                    dulicated = true;
                    QueryResultView(0, "duplicated column " + attribute->getName(), false).show(anOutput);
                }
            }
        });
        if (dulicated) {
            return false;
        }

        return true;
    }

    void Database::select(std::ostream &anOutput, SQLQueryPtr &aSelectQuery) {
        std::string theEntityName = aSelectQuery->getEntityName();
        validateTableExisted(theEntityName);

        SharedEntityPtr theEntity = getEntity(theEntityName);

        //=====================TARGET==================================================
        if (aSelectQuery->isSelectedAll) {
            auto addTargetFromEntity = [&](std::string entityName) {
                validateTableExisted(entityName);
                getEntity(entityName)->eachAttribute([&](Attribute* anAttribute) {
                    aSelectQuery->addTaget(entityName, anAttribute->getName());
                });
            };
            addTargetFromEntity(theEntityName);
            for (auto& joinExpression : aSelectQuery->getListJoin()) {
                addTargetFromEntity(joinExpression.targetEntityName);
            }
        }

        //======================JOIN=====================================================
        std::vector<JoinedTuple> result;
        eachTuple(theEntityName, [&](Tuple aTuple) {
            JoinedTuple joinedTuple;
            Helpers::JoinHandle::addToJoinedTuple(joinedTuple, theEntityName, aTuple);
            result.push_back(joinedTuple);
        });

        JoinedTuple nullLeftTuple;
        Helpers::JoinHandle::addToJoinedTuple(nullLeftTuple, theEntityName, theEntity->baseNullTuple());

        for (auto &joinExpression : aSelectQuery->getListJoin()) {
            std::vector<Tuple> rightTuple;

            validateTableExisted(joinExpression.targetEntityName);
            eachTuple(joinExpression.targetEntityName, [&](Tuple& aTuple) {
                rightTuple.push_back(aTuple);
            });

            result = Helpers::JoinHandle::joinEntity(
                joinExpression,
                result,
                rightTuple,
                nullLeftTuple,
                getEntity(joinExpression.targetEntityName)->baseNullTuple());
        }

        //=================WHERE===========================================================
        int index = 0;
        for (int i = 0; i < result.size(); i++) {
            if (aSelectQuery->getWhereExpression().apply(result[i])) {
                result[index++] = result[i];
            }
        }

        //=================TABLE-VIEW=======================================================
        TableView theTableView(aSelectQuery->headers);
        for (auto &joinedTuple : result) {
            std::vector<std::string> temp;
            for (auto target : aSelectQuery->getTargets()) {
                temp.push_back(Helpers::SqlTypeHandle::covertSqlTypeToString(joinedTuple[target]));
            }
            theTableView.addData(temp);
        }
        theTableView.show(anOutput);
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
}
