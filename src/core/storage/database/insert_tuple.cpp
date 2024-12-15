//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../view/query_result_view.h"
#include "../../view/table_view.h"
#include "../../util/helpers.h"
#include "../../util/errors.h"
#include "../log/log_manager.h"

void Database::insert(
    std::ostream &anOutput,
    std::string aEntityName,
    std::vector<std::pair<std::string, std::string> > &aValues,
    bool hasAtrributeName) {
    LOCK_TABLE_SHARED(aEntityName);

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
            LogManager::getInstance()->appendUndoEntry(name, transactionPtr->transactionID, OperationType::INSERT,
                                                       aEntityName, {}, theTuple);

            Helpers::TupleHandle::addBNodeKey(theTuple, indexingMetadata);
            auto key = Helpers::TupleHandle::genBNodeKey(theTuple, indexingMetadata);
            LockManagerInstance::get(name)->lockRow(transactionPtr.get(), LockMode::EXCLUSIVE, {aEntityName, key});
            setBTree(indexingMetadata, theTuple);
        } else {
            // setBTree(indexingMetadata,
            //          Helpers::TupleHandle::makeSecondaryTuple(theTuple, indexingMetadata,
            //                                                   theEntity->getClusteredKey()));
        }
    });

    UNLOCK_TABLE(aEntityName);
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

            LOCK_TABLE_SHARED(referrenceEntity->getName());
            referrenceEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool _) {
                if (indexingMetadata.keys[0] == referrenceAttributeName && indexingMetadata.keys.size() == 1) {
                    auto key = Helpers::TupleHandle::genBNodeKey(theTuple, indexingMetadata);
                    auto searchPage = searchBtree(indexingMetadata, key);
                    if (searchPage != endLeaf() && Helpers::TupleHandle::getBTreeKey(*searchPage) == key) {
                        throw Errors("Referrence not found");
                    }
                }
            });
            UNLOCK_TABLE(referrenceEntity->getName());
        }
    }
}
