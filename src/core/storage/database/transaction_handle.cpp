//
// Created by chiendd on 15/12/2024.
//

#include "database.h"
#include "../../util/helpers.h"
#include "../log/log_manager.h"

void Database::beginTransaction() {
    transactionPtr = std::make_shared<Transaction>(TransactionState::GROWING, IsolationLevel::REPEATABLE_READ,
                                                   std::this_thread::get_id());
    LogManager::getInstance()->appendUndoEntry(name, transactionPtr->transactionID, OperationType::BEGIN, "", {}, {});
}


void Database::abort() {
    transactionPtr->transactionState = TransactionState::ABORTED;
    undo();
    LockManagerInstance::get(name)->releaseAllLocks(transactionPtr.get());
    transactionPtr = nullptr;
}

void Database::commit() {
    transactionPtr->transactionState = TransactionState::COMMITTED;
    LockManagerInstance::get(name)->releaseAllLocks(transactionPtr.get());
    transactionPtr = nullptr;
}

void Database::undo() {
    LogManager::getInstance()->eachUndoEntry(name, transactionPtr->transactionID, [&](UndoEntry *entry) {
        auto theEntity = getEntity(entry->tableName);

        theEntity->eachIndexing([&](IndexingMetadata &indexingMetadata, bool isClustered) {
            if (isClustered) {
                Tuple tuple = {};
                auto undoInsert = [&]() {
                    tuple = removeBtree(indexingMetadata, Helpers::TupleHandle::genBNodeKey(entry->afterImage, indexingMetadata));
                };

                auto undoDelete = [&]() {
                    for (auto &[attributeName, value] : entry->beforeImage) {
                        tuple[attributeName] = value;
                    }
                    Helpers::TupleHandle::addBNodeKey(tuple, indexingMetadata);
                    setBTree(indexingMetadata, tuple);
                };

                switch (entry->operationType) {
                    case OperationType::INSERT:
                        undoInsert();
                        break;
                    case OperationType::DELETE:
                        undoDelete();
                        break;
                    case OperationType::UPDATE:
                        undoInsert();
                        undoDelete();
                        break;
                    default:
                        break;
                }
            }
        });
    });
}
