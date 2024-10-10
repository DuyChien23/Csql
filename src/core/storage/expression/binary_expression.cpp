//
// Created by chiendd on 10/10/2024.
//

#include "binary_expression.h"
#include "../../util/helpers.h"

Csql::SqlTypes Csql::BinaryExpression::apply(const JoinedTuple &tuple) {
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
