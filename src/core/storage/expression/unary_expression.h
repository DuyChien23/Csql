//
// Created by chiendd on 10/09/2024.
//

#ifndef UNARY_EXPRESSION_H
#define UNARY_EXPRESSION_H
#include "expression.h"

namespace Csql {
    enum class UnaryOperator {
        is_not,
        is_null,
        is_not_null,
    };

    class UnaryExpression : public Expression {
    public:
        UnaryExpression(UnaryOperator _op, Expression* _expression) : op(_op), expression(_expression) {};

        SqlTypes apply(const JoinedTuple &tuple) override {
            SqlTypes value = expression->apply(tuple);
            switch (op) {
                case UnaryOperator::is_not:
                    return std::holds_alternative<SqlNullType>(value) ? !std::get<SqlBoolType>(value) : false;
                case UnaryOperator::is_null:
                    return std::holds_alternative<SqlNullType>;
                default:
                    return !std::holds_alternative<SqlNullType>;
            }
        }
    private:
        UnaryOperator op;
        Expression* expression;
    };
}


#endif //UNARY_EXPRESSION_H
