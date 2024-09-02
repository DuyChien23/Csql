//
// Created by chiendd on 10/09/2024.
//

#ifndef ARITHMETICEXPRESSTION_H
#define ARITHMETICEXPRESSTION_H
#include "expression.h"
#include "../../util/errors.h"

namespace Csql {
    enum class ArithmeticOperator {
        addition,
        subtraction,
        multiplication,
        division,
        modulus,
    };

    class ArithmeticExpression : public Expression {
    public:
        ArithmeticExpression(ArithmeticOperator _op, Expression* _leftOperand, Expression* _rightOperand)
            : op(_op), leftOperand(_leftOperand), rightOperand(_rightOperand) {};

        SqlTypes apply(const JoinedTuple &tuple) override {
            SqlTypes left = leftOperand->apply(tuple);
            SqlTypes right = rightOperand->apply(tuple);
            if (std::holds_alternative<SqlIntType>(left) && std::holds_alternative<SqlIntType>(right)) {
                if (std::get<SqlIntType>(right) == 0) throw Errors(expressionError);

                switch (op) {
                    case ArithmeticOperator::addition:
                        return std::get<SqlIntType>(left) + std::get<SqlIntType>(right);
                    case ArithmeticOperator::subtraction:
                        return std::get<SqlIntType>(left) - std::get<SqlIntType>(right);
                    case ArithmeticOperator::multiplication:
                        return std::get<SqlIntType>(left) * std::get<SqlIntType>(right);
                    case ArithmeticOperator::division:
                        return std::get<SqlIntType>(left) / std::get<SqlIntType>(right);
                    default:
                        return std::get<SqlIntType>(left) % std::get<SqlIntType>(right);
                }
            }

            if ((!std::holds_alternative<SqlIntType>(left) && !std::holds_alternative<SqlFloatType>(left)) ||
                (!std::holds_alternative<SqlIntType>(right) && !std::holds_alternative<SqlFloatType>(right)) ||
                op == ArithmeticOperator::modulus) {
                throw Errors(expressionError);
            }

            SqlFloatType x = std::holds_alternative<SqlFloatType>(left) ? std::get<SqlFloatType>(left) : std::get<SqlIntType>(left);
            SqlFloatType y = std::holds_alternative<SqlFloatType>(right) ? std::get<SqlFloatType>(right) : std::get<SqlIntType>(right);

            if (y == 0) throw Errors(expressionError);

            switch (op) {
                case ArithmeticOperator::addition:
                    return x + y;
                case ArithmeticOperator::subtraction:
                    return x - y;
                case ArithmeticOperator::multiplication:
                    return x * y;
                default:
                    return x / y;
            }
        }
    private:
        ArithmeticOperator op;
        Expression* leftOperand;
        Expression* rightOperand;
    };
}

#endif //ARITHMETICEXPRESSTION_H
