//
// Created by chiendd on 16/12/2024.
//

#include <gtest/gtest.h>

#include "../src/core/storage/log/log_manager.h"
#include "../src/core/util/helpers.h"

TEST(LogManager, ExampleTest0) {
    std::string databaseName = "test";
    Helpers::FolderHandle::deleteIfExist(databaseName, Configs::undoLogDictionary);

    for (int i = 0; i < 10000; i++) {
        uint32_t transactionId = (i % 10 == 0 ? 1 : 2);
        OperationType type = (i == 0 ? OperationType::BEGIN : OperationType::DELETE);
        LogManager::getInstance()->appendUndoEntry(databaseName, transactionId, type, "table", {}, {});
    }

    int cnt = 0;
    LogManager::getInstance()->eachUndoEntry(databaseName, 1, [&](UndoEntry* entry) {
        cnt++;
    });

    EXPECT_EQ(cnt, 999);
}

TEST(LogManager, ExampleTest1) {
    std::string databaseName = "test";
    Helpers::FolderHandle::deleteIfExist(databaseName, Configs::undoLogDictionary);

    Tuple beforeImage = {{"id", SqlIntType(1)}, {"name", SqlVarcharType("chien")}};
    Tuple afterImage = {{"id", SqlIntType(2)}, {"name", SqlVarcharType("chien")}};
    LogManager::getInstance()->appendUndoEntry(databaseName, 1, OperationType::DELETE, "table", beforeImage, afterImage);

    LogManager::getInstance()->eachUndoEntry(databaseName, 1, [&](UndoEntry* entry) {
        EXPECT_EQ(entry->transactionId, 1);
        EXPECT_EQ(entry->operationType, OperationType::DELETE);
        EXPECT_EQ(entry->tableName, "table");
        EXPECT_EQ(entry->beforeImage["id"], beforeImage["id"]);
        EXPECT_EQ(entry->beforeImage["name"], beforeImage["name"]);
        EXPECT_EQ(entry->afterImage["id"], afterImage["id"]);
        EXPECT_EQ(entry->afterImage["name"], afterImage["name"]);
    });

}