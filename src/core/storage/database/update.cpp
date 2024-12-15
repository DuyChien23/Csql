//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../util/helpers.h"
#include "../../view/query_result_view.h"
#include "../expression/other_expression.h"
#include "../log/log_manager.h"

void Database::update(std::ostream &anOutput, const UpdateQueryPtr &aUpdateQuery) {
    LOCK_TABLE_SHARED(aUpdateQuery->getEntityName());

    std::string theEntityName = aUpdateQuery->getEntityName();
    validateTableExisted(theEntityName);

    SharedEntityPtr theEntity = getEntity(theEntityName);

    std::vector<BPlusKey> updateKeys;

    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
            auto iter = beginLeaf(indexingMetadata);

            while (iter != endLeaf()) {
                if (aUpdateQuery->getWhereExpression().apply(Helpers::JoinHandle::covertTupleToJoinedTuple(theEntityName, *iter))) {
                    updateKeys.push_back(Helpers::TupleHandle::genBNodeKey(*iter, indexingMetadata));
                }

                nextLeaf(iter);
            }
        }
    });

    for (auto& key : updateKeys) {
        LockManagerInstance::get(name)->lockRow(transactionPtr.get(), LockMode::EXCLUSIVE, {theEntityName, key});
    }

    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
            for (auto key : updateKeys) {
                auto tuple = removeBtree(indexingMetadata, key);

                Tuple before;
                Tuple after;

                for (auto &a : indexingMetadata.keys) {
                    before[a] = tuple[a];
                    after[a] = tuple[a];
                }

                for (auto &[attributeName, expresstion] : aUpdateQuery->getUpdatesList()) {
                    before[attributeName] = tuple[attributeName];
                    tuple[attributeName] = expresstion->apply(Helpers::JoinHandle::covertTupleToJoinedTuple(theEntityName, tuple));
                    after[attributeName] = tuple[attributeName];
                }

                LogManager::getInstance()->appendUndoEntry(name, transactionPtr->transactionID, OperationType::UPDATE, theEntityName, before, after);

                Helpers::TupleHandle::addBNodeKey(tuple, indexingMetadata);

                setBTree(indexingMetadata, tuple);
            }
        }
    });

    UNLOCK_TABLE(aUpdateQuery->getEntityName());
    saveEntity(theEntity);

    QueryResultView(updateKeys.size(), "Update tuple.").show(anOutput);
}
