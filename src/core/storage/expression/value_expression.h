//
// Created by chiendd on 10/09/2024.
//

#ifndef VALUE_EXPRESSION_H
#define VALUE_EXPRESSION_H
#include "expression.h"

namespace Csql {
    class ValueExpression : public Expression {
    public:
        explicit ValueExpression(SqlTypes _value) : value(std::move(_value)) {};
        ~ValueExpression() = default;

        SqlTypes apply(const JoinedTuple &tuple) override {
            return value;
        }
    private:
        SqlTypes value;
    };
}

#endif //VALUE_EXPRESSION_H
