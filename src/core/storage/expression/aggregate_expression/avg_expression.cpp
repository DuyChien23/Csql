//
// Created by chiendd on 24/12/2024.
//

#include "avg_expression.h"

SqlTypes AvgExpression::apply(const std::vector<JoinedTuple> &tuples) {
    SqlFloatType result = 0;
    for (auto &tuple: tuples) {
        SqlTypes value = expression->apply(tuple);

        if (std::holds_alternative<SqlIntType>(value)) {
            result += std::get<SqlIntType>(value);
        }
        if (std::holds_alternative<SqlFloatType>(value)) {
            result += std::get<SqlFloatType>(value);
        }
    }
    return result / tuples.size();
}
