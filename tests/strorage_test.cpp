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

        SharedEntityPtr getTable(std::string entityName) {
            return getEntity(entityName);
        }

        void readP(const std::string &entityName, uint32_t pageIndex, SharedPagePtr& aPage) {
            readPage(entityName, pageIndex, aPage);
        }

        void writeP(SharedPagePtr aPage) {
            writePage(aPage);
        }
    };
}

void makeFile(std::string path) {
    std::remove(path.c_str());
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

TEST(STORAGE_TEST, TEST_READ_WRITE_PAGE) {
    std::string dirPath = "Databases/a2k24";

    std::filesystem::create_directory("Databases");
    std::filesystem::create_directory(dirPath);

    std::string entityName = "mytest";
    makeFile(dirPath + "/" + entityName + ".csql");

    StorageTest storage_test("a2k24");
    SharedEntityPtr theEntity = std::make_shared<Entity>("a2k24", entityName);
    theEntity->setFirstDataPage(1);
    theEntity->setFirstFreePage(0);
    theEntity->save();
    std::fstream file(dirPath + "/" + entityName + ".csql", MODIFYING_DISK_MODE);
    theEntity->encode(file);

    SharedPagePtr thePage = std::make_shared<SlottedPage>(1, theEntity);

    uint32_t nextPage, beginFreeSpace, endFreeSpace, numSlots;
    thePage->set_next_page(nextPage = 3);

    storage_test.writeP(thePage);

    SharedPagePtr aPage;
    storage_test.readP(entityName, 1, aPage);

    EXPECT_EQ(aPage->get_next_page(), nextPage);
}

