//
// Created by chiendd on 23/12/2024.
//

#include "../src/core/controller/concurrency/lock_manager.h"

#include <gtest/gtest.h>

TEST(LockManager, RunCycleDetection) {
    std::string databaseName = "abc";
    LockManagerInstance::get(databaseName);
    std::vector<TransactionPtr> transactions;

    std::thread newThread(LockManagerInstance::RunCycleDetection);

    sleep(3);
    LockManagerInstance::get(databaseName)->addEdge(1, 2);
    LockManagerInstance::get(databaseName)->addEdge(2, 3);
    LockManagerInstance::get(databaseName)->addEdge(3, 1);

    LockManagerInstance::get(databaseName)->addEdge(4, 5);
    LockManagerInstance::get(databaseName)->addEdge(5, 4);

    newThread.join();
}