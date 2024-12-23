//
// Created by chiendd on 02/12/2024.
//

#include "lock_manager.h"

#include <iostream>
#include <queue>
#include <unordered_set>


std::unordered_map<std::string, LockManager *> LockManagerInstance::instances = {};
std::mutex LockManagerInstance::latch;

LockManager::LockManager() {
}

auto LockManager::lockTable(Transaction *txn, LockMode lockMode, const std::string &tableName) -> bool {
    std::unique_lock lock(table_lock_map_latch);

    if (!tableLockMap.contains(tableName)) {
        tableLockMap[tableName] = std::make_shared<LockRequestQueue>();
    }

    makeWaitVertex(txn->transactionID);

    auto &lockQueue = tableLockMap[tableName];

    auto hasConflict = [&]() -> bool {
        if (!checkVertex(txn->transactionID)) {
            return false;
        }

        for (auto &req: lockQueue->requestQueue) {
            if (txn->transactionID == req->txnId) continue;
            if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                addEdge(txn->transactionID, req->txnId);
                return true;
            }
        }

        std::lock_guard lockRow(row_lock_map_latch);
        for (auto &[rid, que]: rowLockmap) {
            if (rid.first == tableName) {
                for (auto &req: que->requestQueue) {
                    if (txn->transactionID == req->txnId) continue;
                    if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                        addEdge(txn->transactionID, req->txnId);
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

    for (auto &req: lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            if (lockMode == LockMode::EXCLUSIVE) {
                req->lockMode = lockMode;
            }
            return true;
        }
    }

    // txn->holdTableLocks->insert(tableName);
    lockQueue->requestQueue.emplace_back(std::make_shared<LockRequest>(txn->transactionID, lockMode, tableName));
    return true;
}

auto LockManager::unlockTable(Transaction *txn, const std::string &tableName) -> bool {
    std::unique_lock lock(table_lock_map_latch);

    if (!tableLockMap.contains(tableName)) {
        return false;
    }

    auto &lockQueue = tableLockMap[tableName];

    for (auto &req: lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            lockQueue->requestQueue.remove(req);
            removeAllEdge(txn->transactionID);
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

    makeWaitVertex(txn->transactionID);

    auto &lockQueue = rowLockmap[rid];

    auto hasConflict = [&]() {
        if (checkVertex(txn->transactionID)) {
            return false;
        }

        for (auto &req: lockQueue->requestQueue) {
            if (txn->transactionID == req->txnId) continue;
            if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                addEdge(txn->transactionID, req->txnId);
                return true;
            }
        }

        std::lock_guard lockRow(table_lock_map_latch);
        if (tableLockMap.contains(rid.first)) {
            for (auto &req: tableLockMap[rid.first]->requestQueue) {
                if (txn->transactionID == req->txnId) continue;
                if (req->lockMode == LockMode::EXCLUSIVE || lockMode == LockMode::EXCLUSIVE) {
                    addEdge(txn->transactionID, req->txnId);
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

    for (auto &req: lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            if (lockMode == LockMode::EXCLUSIVE) {
                req->lockMode = lockMode;
            }
            return true;
        }
    }

    // txn->holdRowLocks->insert(rid);
    lockQueue->requestQueue.emplace_back(std::make_shared<LockRequest>(txn->transactionID, lockMode, rid));
    return true;
}

auto LockManager::unlockRow(Transaction *txn, const RID &rid) -> bool {
    std::unique_lock lock(row_lock_map_latch);

    if (!rowLockmap.contains(rid)) {
        return false;
    }

    auto &lockQueue = rowLockmap[rid];

    for (auto &req: lockQueue->requestQueue) {
        if (req->txnId == txn->transactionID) {
            lockQueue->requestQueue.remove(req);
            removeAllEdge(txn->transactionID);
            lockQueue->cv_.notify_all();
            return true;
        }
    }

    return true;
}

auto LockManager::releaseAllLocks(Transaction *txn) -> void {
    std::lock_guard tableLock(table_lock_map_latch);
    std::lock_guard rowLock(row_lock_map_latch);

    for (auto &[_, lockQueue]: tableLockMap) {
        for (auto &req: lockQueue->requestQueue) {
            if (req->txnId == txn->transactionID) {
                lockQueue->requestQueue.remove(req);
                break;
            }
        }
        lockQueue->cv_.notify_all();
    }

    for (auto &[_, lockQueue]: rowLockmap) {
        for (auto &req: lockQueue->requestQueue) {
            if (req->txnId == txn->transactionID) {
                lockQueue->requestQueue.remove(req);
                break;
            }
        }
        lockQueue->cv_.notify_all();
    }
}

auto LockManager::makeWaitVertex(txn_id_t t) -> void {
    std::lock_guard lock(waits_for_latch);
    waits_for[t] = {};
}


auto LockManager::addEdge(txn_id_t t1, txn_id_t t2) -> void {
    std::lock_guard lock(waits_for_latch);
    waits_for[t1].insert(t2);
}

auto LockManager::removeEdge(txn_id_t t1, txn_id_t t2) -> void {
    std::lock_guard lock(waits_for_latch);
    waits_for[t1].erase(t2);
}

auto LockManager::removeAllEdge(txn_id_t t2) -> void {
    std::lock_guard lock(waits_for_latch);
    waits_for.erase(t2);
    for (auto &[t1, s]: waits_for) {
        if (s.contains(t2)) {
            s.erase(t2);
        }
    }
}

auto LockManager::checkVertex(txn_id_t t2) -> bool {
    std::lock_guard lock(waits_for_latch);
    return waits_for.contains(t2);
}


auto LockManager::hasCycle(txn_id_t t) -> bool {
    std::lock_guard lock(waits_for_latch);
    std::queue<txn_id_t> q;
    std::unordered_set<txn_id_t> visited;

    q.push(t);
    visited.insert(t);
    while (!q.empty()) {
        auto u = q.front();
        q.pop();

        for (auto v: waits_for[u]) {
            if (!visited.contains(v)) {
                q.push(v);
                visited.insert(v);
            } else {
                return true;
            }
        }
    }
    return false;
}

auto LockManager::RunCycleDetection() -> void {
    for (auto &[t1, s]: waits_for) {
        if (hasCycle(t1)) {
            std::cerr << "Detect cycle! " << " Remove edge from " << t1 << std::endl;
            waits_for.erase(t1);
            break;
        }
    }
}
