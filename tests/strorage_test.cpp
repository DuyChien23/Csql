//
// Created by chiendd on 13/08/2024.
//

#include <gtest/gtest.h>
#include <filesystem>
#include <random>

#include "../src/core/storage/storage.h"
#include "../src/core/storage/expression/other_expression.h"
#include "../src/core/storage/page/b_plus_node/leaf_b_plus_node.h"
#include "../src/core/util/helpers.h"

class StorageTest0 : public Storage, public ::testing::Test {
protected:
    StorageTest0() : Storage("a2k22") {
        auto dbDict = "Databases/" + this->name;
        std::filesystem::remove_all(dbDict);
        std::filesystem::create_directory(dbDict);

        theEntityName = "students";

        std::ofstream f(createEntityPath(theEntityName));
        f.close();

        SharedEntityPtr entity = std::make_shared<Entity>(this->name, theEntityName);
        entity->addIndexingMetadata("id", true);
        entity->eachIndexing([&](IndexingMetadata& indexingMetadata, bool _) {
           indexingMetadata.root = 3;
        });
        saveEntity(entity);

        pageIndex = 13;
        nextLeafPage = 16;
        SharedPagePtr page = std::make_shared<LeafBPlusNode>(pageIndex, entity);
        dynamic_cast<LeafBPlusNode*>(page.get())->nextLeaf = nextLeafPage;
        writePage(page);
    };

    std::string theEntityName;
    uint32_t pageIndex;
    uint32_t nextLeafPage;
};

TEST_F(StorageTest0, GetListEntityNames) {
    auto entityNames = getListEntityNames();
    ASSERT_NE(entityNames, nullptr);
}

TEST_F(StorageTest0, GetFstream) {
    std::fstream& fstream = getFstream(theEntityName);
    ASSERT_TRUE(fstream.is_open());
}

TEST_F(StorageTest0, GetEntity) {
    auto entity = getEntity(theEntityName);
    ASSERT_NE(entity, nullptr);
    uint32_t root;
    entity->eachIndexing([&](IndexingMetadata& indexingMetadata, bool _) {
        root = indexingMetadata.root;
    });
    ASSERT_EQ(root, 3);
}

TEST_F(StorageTest0, CreateKey) {
    std::string key = createKey("test_entity", 1);
    ASSERT_EQ(key, "test_entity#1");
}

TEST_F(StorageTest0, CreateEntityPath) {
    std::string path = createEntityPath("test_entity");
    ASSERT_EQ(path, "Databases/a2k22/test_entity.csql");
}

TEST_F(StorageTest0, ReadPage) {
    SharedPagePtr page;
    readPage(theEntityName, pageIndex, page);
    ASSERT_EQ(page->pageIndex, pageIndex);
    ASSERT_EQ(dynamic_cast<LeafBPlusNode*>(page.get())->nextLeaf, nextLeafPage);
}

class StorageTest1 : public Storage, public ::testing::Test {
protected:
    StorageTest1() : Storage("a2k23") {
        auto dbDict = "Databases/" + this->name;
        std::filesystem::remove_all(dbDict);
        std::filesystem::create_directory(dbDict);

        theEntityName = "students";

        SharedEntityPtr entity = std::make_shared<Entity>(this->name, theEntityName);
        entity->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type)->setPrimary(true));
        entity->addAttribute((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
        entity->addAttribute((new Attribute)->setName("email")->setType(DataTypes::varchar_type)->setUnique(true));

        entity->addIndexingMetadata("id", true);
        entity->addIndexingMetadata("email", false);

        std::ofstream f(createEntityPath(theEntityName));
        f.close();
        saveEntity(entity);


        getEntity(theEntityName)->eachIndexing([&](IndexingMetadata& indexingMetadata, bool isClustered) {
            if (isClustered) {
                auto run = [&](int i) {
                    auto temp = std::to_string(i);

                    Tuple tuple = {
                        std::make_pair(SpecialKey::BTREE_KEY, BPlusKey(i)),
                        std::make_pair("id", SqlIntType(i)),
                        std::make_pair("name", SqlVarcharType("hanh") + temp),
                        std::make_pair("email", SqlVarcharType("chien121803@cvp.vn") + temp)
                    };

                    setBTree(indexingMetadata, tuple);
                };

                std::vector<int> id;
                for (int i = 1; i <= limit; ++i) id.push_back(i);

                std::shuffle(id.begin(), id.end(), std::mt19937(std::random_device()()));

                for (auto i : id) {
                    run(i);
                    if (false) {
                        std::cerr << "======================================: [" + std::to_string(i) + "]" << std::endl;
                        printBtree(indexingMetadata);
                    }
                }
            }
        });


        saveEntity(theEntityName);
    };

    std::string theEntityName;
    int limit = 10000;
};

TEST_F(StorageTest1, SelectBtree) {
    getEntity(theEntityName)->eachIndexing([&](IndexingMetadata& indexingMetadata, bool isClustered) {
        if (isClustered) {
            auto run = [&](int i, bool expected) {
                auto key = BPlusKey(i);
                auto iter = searchBtree(indexingMetadata, key);

                if (expected) {
                    ASSERT_NE(iter, endLeaf());
                    ASSERT_EQ(Helpers::TupleHandle::getBTreeKey((*iter)), key);
                } else {
                    ASSERT_EQ(iter, endLeaf());
                }
            };

            run(0, 0);
            run(limit + 1, 0);
            for (int i = 1; i <= limit; ++i) {
                run(i, 1);
            }
        }
    });
}

TEST_F(StorageTest1, BeginOfBtree) {
    getEntity(theEntityName)->eachIndexing([&](IndexingMetadata& indexingMetadata, bool isClustered) {
        if (isClustered) {
            auto iter = beginLeaf(indexingMetadata);
            ASSERT_NE(iter, endLeaf());

            int i = 1;
            while (iter != endLeaf()) {
                ASSERT_EQ(Helpers::TupleHandle::getBTreeKey(*iter), BPlusKey(i));
                i++;
                iter = nextLeaf(iter);
            }
        }
    });
}

TEST_F(StorageTest1, RemoveBtree) {
    getEntity(theEntityName)->eachIndexing([&](IndexingMetadata& indexingMetadata, bool isClustered) {
        if (isClustered) {
            std::vector<int> id;
            for (int i = 1; i <= limit; ++i) id.push_back(i);
            std::shuffle(id.begin(), id.end(), std::mt19937(std::random_device()()));

          //  printBtree(indexingMetadata);
            for (auto i : id) {
                auto key = BPlusKey(i);
                removeBtree(indexingMetadata, key);
               // printBtree(indexingMetadata);
                auto iter = searchBtree(indexingMetadata, key);
                ASSERT_EQ(iter, endLeaf());
            }
        }
    });
}
