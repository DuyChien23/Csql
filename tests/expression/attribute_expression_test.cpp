//
// Created by chiendd on 11/09/2024.
//

#include "expression_test.h"
#include "../../src/core/storage/expression/attribute_expression.h"

class AttributeExpressionTest : public ::testing::Test {
protected:
    JoinedTuple tuple;
};

// Test case: Attribute exists in JoinedTuple
TEST_F(AttributeExpressionTest, AttributeExists) {
    // Set up the joined tuple with some data
    tuple.insert({{"Entity1", "Attribute1"}, SqlIntType(100)});

    // Create AttributeExpression
    AttributeExpression expr("Entity1", "Attribute1");

    // Apply expression and verify result
    EXPECT_EQ(std::get<SqlIntType>(expr.apply(tuple)), 100);
}

// Test case: Attribute does not exist in JoinedTuple
TEST_F(AttributeExpressionTest, AttributeDoesNotExist) {
    // Create AttributeExpression
    AttributeExpression expr("Entity1", "Attribute1");

    // Expect the method to throw an error
    EXPECT_THROW(expr.apply(tuple), Errors);
}

// Test case: Attribute with float value
TEST_F(AttributeExpressionTest, AttributeFloatValue) {
    // Set up the joined tuple with some data
    tuple.insert({{"Entity1", "Attribute1"}, SqlFloatType(123.45f)});

    // Create AttributeExpression
    AttributeExpression expr("Entity1", "Attribute1");

    // Apply expression and verify result
    EXPECT_EQ(std::get<SqlFloatType>(expr.apply(tuple)), 123.45f);
}

// Test case: Attribute with different entity name
TEST_F(AttributeExpressionTest, DifferentEntityName) {
    // Set up the joined tuple with some data
    tuple.insert({{"Entity1", "Attribute1"},SqlIntType(200)});

    // Create AttributeExpression with different entity name
    AttributeExpression expr("Entity2", "Attribute1");

    // Expect the method to throw an error
    EXPECT_THROW(expr.apply(tuple), Errors);
}

// Test case: Attribute with different attribute name
TEST_F(AttributeExpressionTest, DifferentAttributeName) {
    // Set up the joined tuple with some data
    tuple.insert({{"Entity1", "Attribute1"}, SqlIntType(300)});

    // Create AttributeExpression with different attribute name
    AttributeExpression expr("Entity1", "Attribute2");

    // Expect the method to throw an error
    EXPECT_THROW(expr.apply(tuple), Errors);
}