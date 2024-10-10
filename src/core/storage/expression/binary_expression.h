//
// Created by chiendd on 10/09/2024.
//

#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "expression.h"

namespace Csql {
    enum class BinaryOperator {
        equal,
        not_equal,
        less_than,
        less_than_or_equal,
        greater,
        greater_or_equal,
        logic_or,
        logic_and,
        like,
        not_like,
    };

    class BinaryExpression : public Expression {
    public:
        BinaryExpression(BinaryOperator _op, Expression* _lhs, Expression* _rhs) : op(_op), lhs(_lhs), rhs(_rhs) {};

        SqlTypes apply(const JoinedTuple &tuple) override;
    private:
        BinaryOperator op;
        Expression* lhs;
        Expression* rhs;
    };
}

#endif //BINARY_EXPRESSION_H
