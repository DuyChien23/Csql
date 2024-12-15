//
// Created by chiendd on 22/12/2024.
//

#include "set_expression.h"

#include "../../util/errors.h"
#include "../../util/helpers.h"

SetExpression::SetExpression(SetTypes _type, std::vector<SqlTypes> _list) {
    type = _type;
    list = _list;

    for (int i = 1; i < list.size(); ++i) {
        if (list[i].index() != list[i - 1].index()) {
            throw Errors("Type of operand invalid");
        }
    }

    if (type == SetTypes::range && list.size() != 2) {
        throw Errors("Number operand over");
    }
}

SqlTypes SetExpression::apply(const JoinedTuple &tuple) {
    SqlTypes v = tuple.begin()->second;
    if (!list.empty() && v.index() != list.begin()->index()) throw Errors("Type of operand invalid");

    if (type == SetTypes::range) {
        return Helpers::ExpressionHandle::ExpressionHandle::compareSqlType(list[0], v) != SqlTypeCompareResult::greater
               && Helpers::ExpressionHandle::compareSqlType(v, list[1]) != SqlTypeCompareResult::greater;
    }

    for (auto &element: list) {
        if (Helpers::ExpressionHandle::compareSqlType(v, element) == SqlTypeCompareResult::equal) {
            return true;
        }
    }
    return false;
}
