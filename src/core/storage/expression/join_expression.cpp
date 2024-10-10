//
// Created by chiendd on 11/09/2024.
//

#include "join_expression.h"

#include "binary_expression.h"
#include "value_expression.h"
#include "../../util/errors.h"

namespace Csql {
    JoinExpression::JoinExpression(JoinTypes type, std::string _targetEntityName) {
        joinType = type;
        targetEntityName = std::move(_targetEntityName);
        expression = new ValueExpression(true);
    }

    JoinExpression::JoinExpression(Expression *anExpression) {
        expression = anExpression;
    }


    bool JoinExpression::apply(const JoinedTuple &tuple) const {
        SqlTypes result = expression->apply(tuple);
        if (!std::holds_alternative<SqlBoolType>(result)) throw Errors("Type of Operand invalid");
        return std::get<SqlBoolType>(result);
    }

    void JoinExpression::addAndExpression(Expression *anExpression) {
        expression = new BinaryExpression(BinaryOperator::logic_and, expression, anExpression);
    }

    void JoinExpression::addOrExpression(Expression *anExpression) {
        expression = new BinaryExpression(BinaryOperator::logic_or, expression, anExpression);
    }
}
