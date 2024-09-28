#include "../src/core/storage/database.h"
#include "../src/core/storage/expression/attribute_expression.h"
#include "../src/core/storage/expression/binary_expression.h"
#include "../src/core/storage/expression/value_expression.h"
#include "../src/core/view/table_view.h"
#include "gtest/gtest.h"

std::string dbName = "a2k23";

using namespace  Csql;

Database *intiDatabase() {
    std::string dirPath = "Databases/" + dbName;
    std::filesystem::create_directory("Databases");
    std::filesystem::create_directory(dirPath);
    Database *database = new Database(dbName);
    return database;
}

TEST(MAIN, MAIN_TEST_DROP_TABLE) {
    Database *database = intiDatabase();
    database->dropTable(std::cerr, "students");
    database->dropTable(std::cerr, "courses");
    database->dropTable(std::cerr, "student_course");
}

TEST(MAIN, MAIN_TEST_CREATE_TABLE) {
    Database *database = intiDatabase();

    SharedEntityPtr entity1 = std::make_shared<Entity>(dbName, "students");
    entity1->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type)->setPrimary(true)->setAutoIncrement(true));
    entity1->addAttribute((new Attribute)->setName("first_name")->setType(DataTypes::varchar_type));
    entity1->addAttribute((new Attribute)->setName("last_name")->setType(DataTypes::varchar_type));
    entity1->addAttribute((new Attribute)->setName("birthday")->setType(DataTypes::datetime_type));
    entity1->addAttribute((new Attribute)->setName("email")->setType(DataTypes::varchar_type)->setUnique(true));
    database->createTable(std::cerr, entity1);


    SharedEntityPtr entity2 = std::make_shared<Entity>(dbName, "courses");
    entity2->addAttribute((new Attribute)->setName("id")->setType(DataTypes::int_type)->setPrimary(true)->setAutoIncrement(true));
    entity2->addAttribute((new Attribute)->setName("name")->setType(DataTypes::varchar_type));
    entity2->addAttribute((new Attribute)->setName("code")->setType(DataTypes::varchar_type)->setUnique(true)->setNullable(false));
    database->createTable(std::cerr, entity2);

    SharedEntityPtr entity3 = std::make_shared<Entity>(dbName, "student_course");
    entity3->addAttribute((new Attribute)->setName("student_id")->setType(DataTypes::int_type)->setNullable(false)
        ->setReference((new Reference)->setEntityName("students")->setAttributeName("id")));
    entity3->addAttribute((new Attribute)->setName("course_id")->setType(DataTypes::int_type)->setNullable(false)
        ->setReference((new Reference)->setEntityName("courses")->setAttributeName("id")));
    database->createTable(std::cerr, entity3);
}

TEST(MAIN, MAIN_TEST_SHOW_TABLES) {
    Database *database = intiDatabase();
    database->showTables(std::cerr);
}

TEST(MAIN, MAIN_TEST_DESCRIBE_TABLE) {
    Database *database = intiDatabase();
    database->describeTable(std::cerr, "students");
}

TEST(MAIN, MAIN_TEST_INSERT) {
    Database *database = intiDatabase();
    Tuple tuple1 = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("first_name", SqlVarcharType("Dao")),
        std::make_pair("last_name", SqlVarcharType("Chien")),
        std::make_pair("birthday", SqlDatetimeType(1)),
        std::make_pair("email", SqlVarcharType("neihcr7j23")),
    };

    Tuple tuple2 = {
        std::make_pair("id", SqlIntType(1)),
        std::make_pair("name", SqlVarcharType("DSA")),
        std::make_pair("code", SqlVarcharType("INT123")),
    };

    Tuple tuple3 = {
        std::make_pair("student_id", SqlIntType(2)),
        std::make_pair("course_id", SqlIntType(2)),
    };

   database->insert(std::cerr, "students", tuple1);
   // database->insert(std::cerr, "courses", tuple2);
  //database->insert(std::cerr, "student_course", tuple3);
}

TEST(MAIN, MAIN_TEST_SELECT) {
    Database *database = intiDatabase();
    SQLQueryPtr aSQLQuery = std::make_unique<SQLQuery>();
    // JoinExpression join_expression1(JoinTypes::cross, "students");
    // join_expression1.addAndExpression(
    //     new BinaryExpression(BinaryOperator::equal,
    //         new AttributeExpression("students", "id"),
    //         new AttributeExpression("student_course", "student_id")));
    // aSQLQuery->addJoinExpression(join_expression1);
    aSQLQuery->setEntityName("students");
    database->select(std::cerr, aSQLQuery);
}

TEST(MAIN, MAIN_TEST_DELETE) {
    Database *database = intiDatabase();
    SQLQueryPtr aSQLQuery = std::make_unique<SQLQuery>();
    WhereExpression where_expression;
    where_expression.addAndExpression(new BinaryExpression(BinaryOperator::equal,
            new AttributeExpression("students", "id"),
            new ValueExpression(SqlIntType(2))));
    aSQLQuery->setWhereExpression(where_expression);

    // JoinedTuple joined_tuple;
    //
    // auto key = std::make_pair("students", "id");
    // joined_tuple[key] = SqlIntType(2);


    aSQLQuery->setEntityName("students");
    database->deleteTuples(std::cerr, aSQLQuery);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
