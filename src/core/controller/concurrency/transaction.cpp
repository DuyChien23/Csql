//
// Created by chiendd on 02/12/2024.
//

#include "transaction.h"

#include "../../storage/global_variable/global_variable.h"

Transaction::Transaction(TransactionState ts, IsolationLevel isL, std::thread::id tID) {
    transactionID = GlobalVariable::getInstance()->getTransactionCounter();
    transactionState = ts;
    isolationLevel = isL;
    threadID = tID;

    holdShareLocks = std::make_shared<std::unordered_set<std::string>>();
    holdExclusiveLocks = std::make_shared<std::unordered_set<std::string>>();
}
