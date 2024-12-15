//
// Created by chiendd on 02/12/2024.
//

#include "lock_manager.h"


std::unordered_map<std::string, LockManager*> LockManagerInstance::instances = {};
std::mutex LockManagerInstance::latch;

LockManager::LockManager() : rowLockmap() {}

auto LockManager::lockTable(Transaction *txn, LockMode lockMode, const std::string& tableName) -> bool {
    std::unique_lock lock(table_lock_map_latch);

    if (!tableLockMap.contains(tableName)) {
        tableLockMap[tableName] = std::make_shared<LockRequestQueue>();
    }

    auto &lockQueue = tableLockMap[tableName];

    auto hasConflict = [&]() -> bool {
        for (auto &req : lockQueue->requestQueue) {
            if (txn->transactionID == req->txnId) continue;
            if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                return true;
            }
        }

        std::lock_guard lockRow(row_lock_map_latch);
        for (auto &[rid, que] : rowLockmap) {
            if (rid.first == tableName) {
                for (auto &req : que->requestQueue) {
                    if (txn->transactionID == req->txnId) continue;
                    if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                        return true;
                    }
                }
            }
        }

        return false;
    };

    if (!hasConflict()) {
        lockQueue->cv_.wait(lock, [&hasConflict]() {
            return !hasConflict();
        });
    }

    for (auto &req : lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            if (lockMode == LockMode::EXCLUSIVE) {
                req->lockMode = lockMode;
            }
            return true;
        }
    }

    lockQueue->requestQueue.emplace_back(std::make_shared<LockRequest>(txn->transactionID, lockMode, tableName));
    return true;
}

auto LockManager::unlockTable(Transaction *txn, const std::string &tableName) -> bool {
    std::unique_lock lock(table_lock_map_latch);

    if (!tableLockMap.contains(tableName)) {
        return false;
    }

    auto &lockQueue = tableLockMap[tableName];

    for (auto &req : lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            lockQueue->requestQueue.remove(req);
            lockQueue->cv_.notify_all();
            return true;
        }
    }

    return true;
}

auto LockManager::lockRow(Transaction *txn, LockMode lockMode, const RID &rid) -> bool {
    std::unique_lock lock(row_lock_map_latch);

    if (!rowLockmap.contains(rid)) {
        rowLockmap[rid] = std::make_shared<LockRequestQueue>();
    }

    auto &lockQueue = rowLockmap[rid];

    auto hasConflict = [&]() {
        for (auto &req : lockQueue->requestQueue) {
            if (txn->transactionID == req->txnId) continue;
            if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                return true;
            }
        }

        std::lock_guard lockRow(table_lock_map_latch);
        if (tableLockMap.contains(rid.first)) {
            for (auto &req : tableLockMap[rid.first]->requestQueue) {
                if (txn->transactionID == req->txnId) continue;
                if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                    return true;
                }
            }
        }

        return false;
    };

    if (hasConflict()) {
        lockQueue->cv_.wait(lock, [&hasConflict]() {
            return !hasConflict();
        });
    }

    for (auto &req : lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            if (lockMode == LockMode::EXCLUSIVE) {
                req->lockMode = lockMode;
            }
            return true;
        }
    }

    lockQueue->requestQueue.emplace_back(std::make_shared<LockRequest>(txn->transactionID, lockMode, rid));
    return true;
}

auto LockManager::unlockRow(Transaction *txn, const RID &rid) -> bool {
    std::unique_lock lock(row_lock_map_latch);

    if (!rowLockmap.contains(rid)) {
        return false;
    }

    auto &lockQueue = rowLockmap[rid];

    for (auto &req : lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            lockQueue->requestQueue.remove(req);
            lockQueue->cv_.notify_all();
            return true;
        }
    }

    return true;
}

auto LockManager::releaseAllLocks(Transaction *txn) -> void {
    std::lock_guard tableLock(table_lock_map_latch);
    std::lock_guard rowLock(row_lock_map_latch);

    for (auto &[_, lockQueue] : tableLockMap) {
        for (auto &req : lockQueue->requestQueue) {
            if (req->txnId == txn->transactionID) {
                lockQueue->requestQueue.remove(req);
                break;
            }
        }
        lockQueue->cv_.notify_all();
    }

    for (auto &[_, lockQueue] : rowLockmap) {
        for (auto &req : lockQueue->requestQueue) {
            if (req->txnId == txn->transactionID) {
                lockQueue->requestQueue.remove(req);
                break;
            }
        }
        lockQueue->cv_.notify_all();
    }
}




