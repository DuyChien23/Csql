//
// Created by chiendd on 11/09/2024.
//

#ifndef EXPRESSION_TEST_H
#define EXPRESSION_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/core/storage/expression/expression.h"

class MockExpression : public Expression {
public:
    MOCK_METHOD(SqlTypes, apply, (const JoinedTuple&), (override));
};

#endif //EXPRESSION_TEST_H
