//
// Created by chiendd on 11/09/2024.
//

#include "expression_test.h"
#include "../../src/core/storage/expression/binary_expression.h"

#include "../../src/core/util/errors.h"

// Test Fixture
class BinaryExpressionTest : public ::testing::Test {
protected:
    // Khởi tạo đối tượng MockExpression
    MockExpression lhs;
    MockExpression rhs;
    JoinedTuple tuple;
};

// Test case: Logical OR with boolean values
TEST_F(BinaryExpressionTest, LogicalOrWithBool) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlBoolType(true)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlBoolType(false)));

    BinaryExpression expr(BinaryOperator::logic_or, &lhs, &rhs);
    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: Logical AND with boolean values
TEST_F(BinaryExpressionTest, LogicalAndWithBool) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlBoolType(true)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlBoolType(true)));

    BinaryExpression expr(BinaryOperator::logic_and, &lhs, &rhs);
    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: Logical AND with type mismatch
TEST_F(BinaryExpressionTest, LogicalAndWithTypeMismatch) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlBoolType(true)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));

    BinaryExpression expr(BinaryOperator::logic_and, &lhs, &rhs);
    EXPECT_THROW(expr.apply(tuple), Errors);
}

// Test case: LIKE operator with matching pattern
TEST_F(BinaryExpressionTest, LikeOperatorMatch) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlVarcharType("Hello")));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlVarcharType("He%")));

    BinaryExpression expr(BinaryOperator::like, &lhs, &rhs);
    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: Equality comparison for integers
TEST_F(BinaryExpressionTest, EqualOperatorInt) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));

    BinaryExpression expr(BinaryOperator::equal, &lhs, &rhs);
    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), true);
}

// Test case: Inequality comparison for floats
TEST_F(BinaryExpressionTest, NotEqualOperatorFloat) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(10.5f)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(10.5f)));

    BinaryExpression expr(BinaryOperator::not_equal, &lhs, &rhs);
    EXPECT_EQ(std::get<SqlBoolType>(expr.apply(tuple)), false);
}

// Test case: Comparison with unsupported types
TEST_F(BinaryExpressionTest, CompareUnsupportedTypes) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlVarcharType("Hello")));

    BinaryExpression expr(BinaryOperator::equal, &lhs, &rhs);
    EXPECT_THROW(expr.apply(tuple), Errors);
}

// Test case: Division by zero for unsupported operation
TEST_F(BinaryExpressionTest, UnsupportedOperation) {
    EXPECT_CALL(lhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(rhs, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(0)));

    BinaryExpression expr(BinaryOperator::logic_or, &lhs, &rhs); // Invalid operation for integers
    EXPECT_THROW(expr.apply(tuple), Errors);
}