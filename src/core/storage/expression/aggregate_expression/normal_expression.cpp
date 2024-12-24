//
// Created by chiendd on 24/12/2024.
//

#include "normal_expression.h"

#include "../attribute_expression.h"

NormalExpression::NormalExpression(std::string tableName, std::string attributeName) : TargetExpression(
    new AttributeExpression(std::move(tableName), std::move(attributeName)), normal, tableName, attributeName) {
};

SqlTypes NormalExpression::apply(const std::vector<JoinedTuple> &tuples) {
    // if (tuples.size() != 1) throw Errors("Ambiguous attribute");
    return expression->apply(tuples[0]);
}
