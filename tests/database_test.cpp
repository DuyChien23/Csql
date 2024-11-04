//
// Created by chiendd on 31/10/2024.
//

#include "../src/core/storage/database.h"

#include <math.h>
#include <gtest/gtest.h>

#include "../src/core/util/errors.h"

using namespace Csql;

class DatabaseTest : public Database, public ::testing::Test {
public:
    DatabaseTest() : Database("database_a2k22") {
        theEntityName = "students";
    }

    void resetDB() {
        auto dbDict = "Databases/" + this->name;
        std::filesystem::remove_all(dbDict);
        std::filesystem::create_directory(dbDict);
    }

    void createT() {
        SharedEntityPtr entity = std::make_shared<Entity>(this->name, theEntityName);
        entity->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type)->setPrimary(true));
        entity->addAttribute((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
        entity->addAttribute((new Attribute)->setName("email")->setType(DataTypes::varchar_type)->setUnique(true));

        entity->addIndexingMetadata("id", true);
        entity->addIndexingMetadata("email", false);

        createTable(std::cerr, entity);
    }

    void insertT() {
        std::vector<std::pair<std::string, std::string>> v({{"id", "1"}, {"name", "chien"}, {"email", "chien121803@gmail.com"}});
        insert(std::cerr, theEntityName, v, true);
    }

    void selectT() {
        SQLQueryPtr query = std::make_unique<SQLQuery>();
        query->setEntityName(theEntityName);

        select(std::cerr, query);
    }

    std::string theEntityName;
};

TEST_F(DatabaseTest, CreateTable) {
    try {
        resetDB();
        createT();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, InsertTable) {
    try {
        resetDB();
        createT();
        insertT();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, SelectTable) {
    try {
        resetDB();
        createT();
        insertT();
        selectT();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}