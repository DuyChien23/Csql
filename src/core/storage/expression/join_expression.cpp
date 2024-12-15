//
// Created by chiendd on 11/09/2024.
//

#include "join_expression.h"

#include "binary_expression.h"
#include "../../util/errors.h"

JoinExpression::JoinExpression(Expression *anExpression, JoinTypes aJoinType, std::string aTargetEntityName) {
    expression = anExpression;
    joinType = aJoinType;
    targetEntityName = std::move(aTargetEntityName);
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
