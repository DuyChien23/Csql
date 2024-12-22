//
// Created by chiendd on 31/10/2024.
//

#include "../src/core/storage/database/database.h"

#include <math.h>
#include <gtest/gtest.h>

#include "../src/core/storage/expression/attribute_expression.h"
#include "../src/core/storage/expression/binary_expression.h"
#include "../src/core/storage/expression/value_expression.h"
#include "../src/core/util/errors.h"
#include "../src/core/util/helpers.h"
#include "../third_party/json/single_include/nlohmann/json.hpp"

class DatabaseTest : public Database, public ::testing::Test {
public:
    DatabaseTest() : Database("database_a2k22") {
        theEntityName = "students";
    }

    void resetDB() {
        auto dbDict = "Databases/" + this->name;
        std::filesystem::remove_all(dbDict);
        std::filesystem::create_directory(dbDict);

        auto undoDict = "UndoLogs/" + this->name;
        std::filesystem::remove_all(undoDict);
        std::filesystem::create_directory(undoDict);
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

    void insertT(int id) {
        std::string t = std::to_string(id);
        std::vector<std::pair<std::string, std::string> > v({
            {"id", t}, {"name", "chien"}, {"email", "chien" + t + "@gmail.com"}
        });
        insert(std::cerr, theEntityName, v, true);
    }

    void selectT() {
        SQLQueryPtr query = std::make_unique<SQLQuery>();
        query->setEntityName(theEntityName);

        select(std::cerr, query);
    }

    void updateT(int id, std::string name, std::string email) {
        UpdateQueryPtr query = std::make_unique<UpdateQuery>();
        query->setEntityName(theEntityName);
        query->addUpdate("name", new ValueExpression(name));
        query->addUpdate("email", new ValueExpression(email));
        update(std::cerr, query);
    }

    void deleteT(int id) {
        SQLQueryPtr query = std::make_unique<SQLQuery>();
        query->setEntityName(theEntityName);
        Expression *theExpression = new BinaryExpression(BinaryOperator::equal,
                                                         new AttributeExpression(theEntityName, "id"),
                                                         new ValueExpression(SqlIntType(id)));
        query->setWhereExpression(WhereExpression(theExpression));
        deleteTuples(std::cerr, query);
    }

    std::string theEntityName;
};

TEST_F(DatabaseTest, CreateTable) {
    try {
        resetDB();
        beginTransaction();
        createT();
        commit();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, InsertTable) {
    try {
        resetDB();
        beginTransaction();
        createT();
        commit();

        beginTransaction();
        insertT(1);
        commit();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, SelectTable) {
    try {
        resetDB();
        beginTransaction();
        createT();
        commit();

        beginTransaction();
        insertT(1);
        commit();

        beginTransaction();
        insertT(2);
        insertT(3);
        selectT();
        abort();

        beginTransaction();
        selectT();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, DeleteTuple) {
    try {
        resetDB();
        beginTransaction();
        createT();
        commit();

        beginTransaction();
        insertT(1);
        selectT();
        commit();

        beginTransaction();
        deleteT(1);
        selectT();
        abort();

        beginTransaction();
        selectT();
        commit();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}

TEST_F(DatabaseTest, UpdateTable) {
    try {
        resetDB();
        beginTransaction();
        createT();
        commit();

        beginTransaction();
        insertT(1);
        selectT();
        commit();

        beginTransaction();
        updateT(1, "hanh", "dangiu");
        selectT();
        abort();

        beginTransaction();
        selectT();
        commit();
    } catch (Errors e) {
        std::cerr << e.what() << '\n';
        EXPECT_TRUE(false);
    }
}
