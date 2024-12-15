//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../view/query_result_view.h"
#include "../../util/helpers.h"
#include "../log/log_manager.h"

void Database::deleteTuples(std::ostream &anOutput, const SQLQueryPtr &aDeleteQuery) {
    LOCK_TABLE_SHARED(aDeleteQuery->getEntityName());
    std::string theEntityName = aDeleteQuery->getEntityName();
    validateTableExisted(theEntityName);

    SharedEntityPtr theEntity = getEntity(theEntityName);

    std::vector<std::pair<IndexingMetadata&, BPlusKey> > deleteKeys;

    theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
        if (isClustered) {
            auto iter = beginLeaf(indexingMetadata);

            while (iter != endLeaf()) {
                if (aDeleteQuery->getWhereExpression().apply(
                    Helpers::JoinHandle::covertTupleToJoinedTuple(theEntityName, *iter))) {

                    auto key = Helpers::TupleHandle::genBNodeKey(*iter, indexingMetadata);
                    deleteKeys.push_back({indexingMetadata, key});
                }

                nextLeaf(iter);
            }
        }
    });

    bool flag = true;
    for (auto &[im, key]: deleteKeys) {
        flag &= LockManagerInstance::get(name)->
                lockRow(transactionPtr.get(), LockMode::EXCLUSIVE, {im.entityName, key});
    }

    if (!flag) {
        throw Errors("Can't lock row in exclusive mode");
    }

    for (auto &[im, key]: deleteKeys) {
        auto tuple = removeBtree(im, key);
        tuple.erase(SpecialKey::BTREE_KEY);
        LogManager::getInstance()->appendUndoEntry(name, transactionPtr->transactionID, OperationType::DELETE,
                                                   im.entityName, tuple, {});
    }

    UNLOCK_TABLE(aDeleteQuery->getEntityName());
    saveEntity(theEntity);

    QueryResultView(1, "delete tuple").show(anOutput);
}
