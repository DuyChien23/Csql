//
// Created by chiendd on 11/09/2024.
//

#include "expression_test.h"
#include "../../src/core/storage/expression/arithmetic_expresstion.h"

class ArithmeticExpressionTest : public ::testing::Test {
protected:
    MockExpression left;
    MockExpression right;
    JoinedTuple tuple;
};

// Test case: Addition with integers
TEST_F(ArithmeticExpressionTest, AdditionInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(5)));

    ArithmeticExpression expr(ArithmeticOperator::addition, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlIntType>(result), 15);
}

// Test case: Subtraction with integers
TEST_F(ArithmeticExpressionTest, SubtractionInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(4)));

    ArithmeticExpression expr(ArithmeticOperator::subtraction, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlIntType>(result), 6);
}

// Test case: Multiplication with integers
TEST_F(ArithmeticExpressionTest, MultiplicationInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(3)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(4)));

    ArithmeticExpression expr(ArithmeticOperator::multiplication, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlIntType>(result), 12);
}

// Test case: Division with integers
TEST_F(ArithmeticExpressionTest, DivisionInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(2)));

    ArithmeticExpression expr(ArithmeticOperator::division, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlIntType>(result), 5);
}

// Test case: Modulus with integers
TEST_F(ArithmeticExpressionTest, ModulusInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(3)));

    ArithmeticExpression expr(ArithmeticOperator::modulus, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlIntType>(result), 1);
}

// Test case: Addition with floats
TEST_F(ArithmeticExpressionTest, AdditionFloat) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(10.5)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(5.5)));

    ArithmeticExpression expr(ArithmeticOperator::addition, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlFloatType>(result), 16.0);
}

// Test case: Subtraction with floats
TEST_F(ArithmeticExpressionTest, SubtractionFloat) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(10.5)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(2.5)));

    ArithmeticExpression expr(ArithmeticOperator::subtraction, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlFloatType>(result), 8.0);
}

// Test case: Multiplication with floats
TEST_F(ArithmeticExpressionTest, MultiplicationFloat) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(2.5)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(4.0)));

    ArithmeticExpression expr(ArithmeticOperator::multiplication, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlFloatType>(result), 10.0);
}

// Test case: Division with floats
TEST_F(ArithmeticExpressionTest, DivisionFloat) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(10.5)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlFloatType(2.5)));

    ArithmeticExpression expr(ArithmeticOperator::division, &left, &right);
    auto result = expr.apply(tuple);

    ASSERT_EQ(std::get<SqlFloatType>(result), 4.2);
}

// Test case: Division by zero with integers
TEST_F(ArithmeticExpressionTest, DivisionByZeroInt) {
    EXPECT_CALL(left, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(10)));
    EXPECT_CALL(right, apply(::testing::Ref(tuple))).WillOnce(::testing::Return(SqlIntType(0)));

    ArithmeticExpression expr(ArithmeticOperator::division, &left, &right);
    EXPECT_THROW(expr.apply(tuple), Errors);
}