//
// Created by chiendd on 10/09/2024.
//

#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "expression.h"
#include "../../util/errors.h"
#include "../../util/helpers.h"

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

        SqlTypes apply(const JoinedTuple &tuple) override {
            SqlTypes left = lhs->apply(tuple);
            SqlTypes right = rhs->apply(tuple);

            if (op == BinaryOperator::logic_or || op == BinaryOperator::logic_and) {
                if (!std::holds_alternative<SqlBoolType>(left) || !std::holds_alternative<SqlBoolType>(right)) {
                    throw Errors("Type of operand invalid");
                }

                if (op == BinaryOperator::logic_or) {
                    return std::get<SqlBoolType>(left) || std::get<SqlBoolType>(right);
                } else {
                    return std::get<SqlBoolType>(left) && std::get<SqlBoolType>(right);
                }
            }

            if (op == BinaryOperator::like || op == BinaryOperator::not_like) {
                return static_cast<SqlBoolType>((op == BinaryOperator::not_like) ^ Helpers::ExpressionHandle::likeMatch(left, right));
            }

            SqlTypeCompareResult compareResult = Helpers::ExpressionHandle::ExpressionHandle::compareSqlType(left, right);
            if (compareResult == SqlTypeCompareResult::cant_compare) throw Errors("Can't execute expression");

            switch (op) {
                case BinaryOperator::equal:
                    return compareResult == SqlTypeCompareResult::equal;
                case BinaryOperator::not_equal:
                    return compareResult != SqlTypeCompareResult::equal;
                case BinaryOperator::greater:
                    return compareResult == SqlTypeCompareResult::greater;
                case BinaryOperator::greater_or_equal:
                    return compareResult != SqlTypeCompareResult::less_than;
                case BinaryOperator::less_than:
                    return compareResult == SqlTypeCompareResult::less_than;
                case BinaryOperator::less_than_or_equal:
                    return compareResult != SqlTypeCompareResult::greater;
                default:
                    throw Errors("Expression: other case");
            }
        }
    private:
        BinaryOperator op;
        Expression* lhs;
        Expression* rhs;
    };
}

#endif //BINARY_EXPRESSION_H
