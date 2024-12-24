//
// Created by chiendd on 24/12/2024.
//

#include "sum_expression.h"

SqlTypes SumExpression::apply(const std::vector<JoinedTuple> &tuples) {
    SqlIntType result = 0;
    for (auto &tuple: tuples) {
        result += std::get<SqlIntType>(expression->apply(tuple));
    }
    return result;
}
