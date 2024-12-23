//
// Created by chiendd on 02/12/2024.
//

#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <thread>

using txn_id_t = int;

enum class TransactionState { GROWING, SHRINKING, COMMITTED, ABORTED };

enum class IsolationLevel { READ_COMMITTED, REPEATABLE_READ, SERIALIZABLE };

class Transaction {
public:
    Transaction(TransactionState ts, IsolationLevel isL, std::thread::id tID);

    ~Transaction()=default;

    int transactionID;
    TransactionState    transactionState;
    IsolationLevel      isolationLevel;
    std::thread::id     threadID;
    //
    // std::shared_ptr<std::unordered_set<std::string>> holdTableLocks = nullptr;
    // std::shared_ptr<std::unordered_set<RID>> holdRowLocks = nullptr;
};

using TransactionPtr= std::shared_ptr<Transaction>;

#endif //TRANSACTION_H
