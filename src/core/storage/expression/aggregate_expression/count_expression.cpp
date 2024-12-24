//
// Created by chiendd on 24/12/2024.
//

#include "count_expression.h"

#include "../../../util/helpers.h"

CountExpression::CountExpression(Expression *exp) : TargetExpression(exp, count) {
};

SqlTypes CountExpression::apply(const std::vector<JoinedTuple> &tuples) {
    SqlIntType cnt = 0;
    for (auto &tuple: tuples) {
        if (!std::holds_alternative<SqlNullType>(expression->apply(tuple))) {
            cnt++;
        }
    }
    return cnt;
}

