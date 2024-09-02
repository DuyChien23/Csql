//
// Created by chiendd on 13/08/2024.
//

#include <gtest/gtest.h>
#include <filesystem>

#include "../src/core/storage/storage.h"

using namespace Csql;

namespace Csql {
    class StorageTest : Storage {
    public:
        StorageTest(std::string storageName) : Storage(storageName) {};

        std::vector<std::string>* getListEntityNamesForTest() {
            return getListEntityNames();
        };
    };
}

void makeFile(std::string path) {
    std::ofstream file1(path);
    file1.close();
}

TEST(STORAGE_TEST, TEST_GET_LIST_ENTITY_NAMES) {
    std::string dirPath = "Databases/a2k22";

    std::filesystem::create_directory("Databases");
    std::filesystem::create_directory(dirPath);

    std::vector<std::string> sampleNames = {"student", "score", "subject"};

    for (const auto &entityName : sampleNames) {
        makeFile(dirPath + "/" + entityName + ".csql");
    }

    StorageTest storage_test("a2k22");

    std::vector<std::string>* actualNames = storage_test.getListEntityNamesForTest();

    sort(sampleNames.begin(), sampleNames.end());
    sort(actualNames->begin(), actualNames->end());

    EXPECT_EQ(sampleNames.size(), actualNames->size());

    for (int i = 0; i < sampleNames.size(); ++i) {
        EXPECT_EQ(sampleNames[i], actualNames->at(i));
    }
}