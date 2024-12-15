//
// Created by chiendd on 10/10/2024.
//

#include "other_expression.h"
#include "../../util/helpers.h"

OtherExpression::OtherExpression(OtherOperator _op, Expression *_lhs, Expression *_rhs) {
    op = _op;
    lhs = _lhs;
    rhs = _rhs;
};

SqlTypes OtherExpression::apply(const JoinedTuple &tuple) {
    SqlTypes v = lhs->apply(tuple);
    JoinedTuple t;
    t.insert({{"", ""}, v});

    if (op == OtherOperator::in || op == OtherOperator::not_in) {
        return rhs->apply(t);
    }

    return !std::get<SqlBoolType>(rhs->apply(t));
}
