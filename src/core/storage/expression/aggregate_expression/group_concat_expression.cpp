//
// Created by chiendd on 24/12/2024.
//

#include "group_concat_expression.h"

#include "../../../util/helpers.h"

SqlTypes GroupConcatExpression::apply(const std::vector<JoinedTuple> &tuples) {
    std::string result = "";
    for (int i = 0; i < tuples.size(); ++i) {
        if (i != 0) {
            result += split;
        }
        result += Helpers::SqlTypeHandle::covertSqlTypeToString(expression->apply(tuples[i]));
    }
    return result;
}
