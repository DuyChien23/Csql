//
// Created by chiendd on 10/09/2024.
//

#include "where_expression.h"

#include "binary_expression.h"
#include "value_expression.h"
#include "../../util/errors.h"

namespace Csql {
    WhereExpression::WhereExpression() {
        expression = new ValueExpression(true);
    }

    bool WhereExpression::apply(const JoinedTuple &tuple) const {
        SqlTypes result = expression->apply(tuple);
        if (!std::holds_alternative<SqlBoolType>(result)) throw Errors(expressionError);

        return std::get<SqlBoolType>(result);
    }

    void WhereExpression::addAndExpression(Expression* anExpression) {
        expression = new BinaryExpression(BinaryOperator::logic_and, expression, anExpression);
    }

    void WhereExpression::addOrExpression(Expression* anExpression) {
        expression = new BinaryExpression(BinaryOperator::logic_or, expression, anExpression);
    }
}
