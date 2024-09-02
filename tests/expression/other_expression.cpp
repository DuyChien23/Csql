//
// Created by chiendd on 11/09/2024.
//

#include "expression_test.h"

class OtherExpressionTest : public ::testing::Test {
protected:
    // Khởi tạo đối tượng MockExpression
    MockExpression value;
    JoinedTuple tuple;
};

// Test case: BETWEEN operator with valid range
TEST_F(OtherExpressionTest, BetweenOperatorValidRange) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(15)));

    std::vector<SqlTypes> range = {SqlIntType(10), SqlIntType(20)};
    OtherExpression expr(OtherOperator::between, &value, range);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: BETWEEN operator with value outside range
TEST_F(OtherExpressionTest, BetweenOperatorOutsideRange) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(25)));

    std::vector<SqlTypes> range = {SqlIntType(10), SqlIntType(20)};
    OtherExpression expr(OtherOperator::between, &value, range);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), false);
}

// Test case: NOT BETWEEN operator with valid range
TEST_F(OtherExpressionTest, NotBetweenOperatorValidRange) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(25)));

    std::vector<SqlTypes> range = {SqlIntType(10), SqlIntType(20)};
    OtherExpression expr(OtherOperator::not_between, &value, range);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: NOT BETWEEN operator with value within range
TEST_F(OtherExpressionTest, NotBetweenOperatorWithinRange) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(15)));

    std::vector<SqlTypes> range = {SqlIntType(10), SqlIntType(20)};
    OtherExpression expr(OtherOperator::not_between, &value, range);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), false);
}

// Test case: IN operator with value in list
TEST_F(OtherExpressionTest, InOperatorValueInList) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(15)));

    std::vector<SqlTypes> list = {SqlIntType(10), SqlIntType(15), SqlIntType(20)};
    OtherExpression expr(OtherOperator::in, &value, list);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: IN operator with value not in list
TEST_F(OtherExpressionTest, InOperatorValueNotInList) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(25)));

    std::vector<SqlTypes> list = {SqlIntType(10), SqlIntType(15), SqlIntType(20)};
    OtherExpression expr(OtherOperator::in, &value, list);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), false);
}

// Test case: NOT IN operator with value in list
TEST_F(OtherExpressionTest, NotInOperatorValueInList) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(15)));

    std::vector<SqlTypes> list = {SqlIntType(10), SqlIntType(15), SqlIntType(20)};
    OtherExpression expr(OtherOperator::not_in, &value, list);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), false);
}

// Test case: NOT IN operator with value not in list
TEST_F(OtherExpressionTest, NotInOperatorValueNotInList) {
    EXPECT_CALL(value, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(25)));

    std::vector<SqlTypes> list = {SqlIntType(10), SqlIntType(15), SqlIntType(20)};
    OtherExpression expr(OtherOperator::not_in, &value, list);

    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: Constructor throws exception on invalid type list
TEST_F(OtherExpressionTest, ConstructorThrowsExceptionOnInvalidTypeList) {
    std::vector<SqlTypes> invalidList = {SqlIntType(10), SqlFloatType(15.5)};
    EXPECT_THROW(OtherExpression expr(OtherOperator::between, &value, invalidList), Errors);
}

// Test case: Constructor throws exception on invalid BETWEEN range size
TEST_F(OtherExpressionTest, ConstructorThrowsExceptionOnInvalidBetweenRangeSize) {
    std::vector<SqlTypes> invalidRange = {SqlIntType(10)}; // Only one value
    EXPECT_THROW(OtherExpression expr(OtherOperator::between, &value, invalidRange), Errors);
}