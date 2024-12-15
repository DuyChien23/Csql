//
// Created by chiendd on 02/12/2024.
//

#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H
#include <condition_variable>
#include <list>
#include <thread>
#include <unordered_map>
#include <atomic>

#include "transaction.h"
#include "../../util/b_plus_key.h"

using RID = std::pair<std::string, BPlusKey>;

namespace std {
    template <>
    struct hash<RID> {
        size_t operator()(const RID& rid) const {
            size_t hash1 = std::hash<std::string>()(rid.first);
            size_t hash2 = std::hash<BPlusKey>()(rid.second);
            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };
}

enum class LockMode { SHARED, EXCLUSIVE, INTENTION_SHARED, INTENTION_EXCLUSIVE, SHARED_INTENTION_EXCLUSIVE };

class LockRequest {
public:
    LockRequest(txn_id_t txn_id, LockMode lock_mode, std::string _tableName) /** Table lock request */
      : txnId(txn_id), lockMode(lock_mode), tableName(std::move(_tableName)) {}
    LockRequest(txn_id_t _txnId, LockMode _lockMode, RID _rid)
        : txnId(_txnId), lockMode(_lockMode), rid(std::move(_rid)) {};

    txn_id_t          txnId;
    LockMode          lockMode;
    std::string       tableName;
    RID               rid;
    bool              granted=false;
};

class LockRequestQueue {
public:
    std::list<std::shared_ptr<LockRequest>> requestQueue;
    std::condition_variable cv_;
    std::mutex latch;
};

class LockManager {
public:
    LockManager();

    ~LockManager() = default;

    auto lockTable(Transaction *txn, LockMode lockMode, const std::string &tableName) -> bool;
    auto unlockTable(Transaction *txn, const std::string &tableName) -> bool;

    auto lockRow(Transaction *txn, LockMode lockMode, const RID &rid) -> bool;
    auto unlockRow(Transaction *txn, const RID &rid) -> bool;

    auto releaseAllLocks(Transaction *txn) -> void;

private:
    std::unordered_map<std::string, std::shared_ptr<LockRequestQueue>> tableLockMap;
    std::mutex table_lock_map_latch;

    std::unordered_map<RID, std::shared_ptr<LockRequestQueue>> rowLockmap;
    std::mutex row_lock_map_latch;

    std::atomic<bool> enable_cycle_detection;
    std::thread *cycle_detection_thread;
    /** Waits-for graph representation. */
    std::unordered_map<txn_id_t, std::vector<txn_id_t>> waits_for;
    std::mutex waits_for_latch;
};

class LockManagerInstance {
public:
    static LockManager* &get(const std::string& databaseName) {
        std::lock_guard lock(latch);
        if (!instances.contains(databaseName)) {
            instances[databaseName] = new LockManager();
        }
        return instances[databaseName];
    }

    static void remove(const std::string& databaseName) {
        std::lock_guard lock(latch);
        if (instances.contains(databaseName)) {
            delete instances[databaseName];
            instances.erase(databaseName);
        }
    }
private:
    static std::unordered_map<std::string, LockManager*> instances;
    static std::mutex latch;
};

#endif //LOCK_MANAGER_H