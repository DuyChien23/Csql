//
// Created by chiendd on 21/12/2024.
//

#include "../../src/core/controller/tokenizer/expression_parser.h"

#include <gtest/gtest.h>

Expression* createExpression(std::string sql) {
    std::istringstream iss(sql);
    Tokenizer* tokenizer = new Tokenizer(iss);
    tokenizer->tokenize();
    ExpressionParser expressionParser(tokenizer);
    return expressionParser.parse();
}

JoinedTuple theTuple;
/**
 *  switch (type):
 *      case 0: SqlBoolType
 *      case 1: SqlIntType
 */
void check(std::string sql, int type = 0) {
    try {
        std::cout << "Running ["<< sql << "]..." << std::endl;
        auto expression = createExpression(sql + ";");
        auto result = expression->apply(theTuple);

        if (type == 0) {
            EXPECT_EQ(std::get<SqlBoolType>(result), SqlBoolType(true));
        } else if (type == 1) {
            EXPECT_EQ(std::get<SqlIntType>(result), SqlIntType(4));
        } else {
            EXPECT_EQ(std::get<SqlVarcharType>(result), SqlVarcharType("my name is chien"));
        }
    } catch (Errors e) {
        EXPECT_TRUE(false);
        std::cerr << sql << " got errors " << e.what() << std::endl;
    } catch (...) {
        EXPECT_TRUE(false);
        std::cerr << sql << " got errors " << std::endl;
    }
}


TEST(ExpressionParser, SampleTest) {
    theTuple[{"abc", "id"}] = SqlIntType(1);
    theTuple[{"abc", "name"}] = SqlVarcharType("my name is ");

    check("2 > 1");
    check("1 < 2");
    check("1 <> 2");
    check("1 < 2 AND 2 > 1");
    check("1 < 2 OR 2 < 1");
    check("1 > 2 OR 2 > 1");
    check("abc.id + 1 = 2");
    check("abc.id * 2 + 2", 1);
    check("1 < 2 AND 2 > 1 AND abc.id = 1");
    check("abc.id * 2 + 2 > 3 OR abc.id < 0 AND 1 < 2");
    check("NOT (NOT (abc.id = 1 AND 2 > 1))");
    check("abc.id * 3 + 5 >= 8 AND 1 < 2 OR abc.id = 1");
    check("abc.id > 0 AND (1 < 2 OR abc.id = 1) AND NOT (abc.id * 2 <> 2)");
    check("abc.id * 2 + 2 - 1 = 3 OR abc.id <> 1 AND 3 < 4");
    check("abc.id > 0 AND (abc.id * 3 + 1 > 4 OR (abc.id = 1 AND 2 = 2))");
    check("(abc.id + 1) * 2 = 4 AND (abc.id > 0 OR abc.id <> 1)");
    check("(abc.id + 1) * 2 = 4 AND (abc.id > 0 OR abc.id <> 1)");

    check("abc.name like 'my%'");
    check("abc.name not like 'my mam%'");
    check("abc.name like 'm%nam%'");
    check("abc.name like 'm%nam%s '");

    check("concat('123', '456') = '123456'");
    check("concat_ws('123', 'a', 'b') = 'a123b'");

}