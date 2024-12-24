//
// Created by chiendd on 24/12/2024.
//

#ifndef NORMAL_EXPRESSION_H
#define NORMAL_EXPRESSION_H
#include "target_expression.h"

class NormalExpression : public TargetExpression {
public:
    NormalExpression(std::string tableName, std::string attributeName);

    ~NormalExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override;
};



#endif //NORMAL_EXPRESSION_H
