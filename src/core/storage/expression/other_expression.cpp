//
// Created by chiendd on 10/10/2024.
//

#include "other_expression.h"
#include "../../util/errors.h"
#include "../../util/helpers.h"

OtherExpression::OtherExpression(OtherOperator _op, Expression *_value, std::vector<SqlTypes> _list) {
    op = _op;
    value = _value;
    list = std::move(_list);

    for (int i = 1; i < list.size(); ++i) {
        if (list[i].index() != list[i - 1].index()) {
            throw Errors("Type of operand invalid");
        }
    }

    if ((op == OtherOperator::between || op == OtherOperator::not_between) && list.size() != 2) {
        throw Errors("Number operand over");
    }
};

SqlTypes OtherExpression::apply(const JoinedTuple &tuple) {
    SqlTypes v = value->apply(tuple);
    if (!list.empty() && v.index() != list.begin()->index()) throw Errors("Type of operand invalid");

    if (op == OtherOperator::between || op == OtherOperator::not_between) {
        return static_cast<SqlBoolType>((op == OtherOperator::not_between) ^
                                        (Helpers::ExpressionHandle::ExpressionHandle::compareSqlType(list[0], v) !=
                                         SqlTypeCompareResult::greater
                                         && Helpers::ExpressionHandle::compareSqlType(v, list[1]) !=
                                         SqlTypeCompareResult::greater));
    } else {
        bool is_in = false;
        for (auto &element: list) {
            if (Helpers::ExpressionHandle::compareSqlType(v, element) == SqlTypeCompareResult::equal) {
                is_in = true;
            }
        }
        return static_cast<SqlBoolType>((op == OtherOperator::not_in) ^ is_in);
    }
}
