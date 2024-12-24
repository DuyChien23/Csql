//
// Created by chiendd on 24/12/2024.
//

#ifndef COUNT_EXPRESSION_H
#define COUNT_EXPRESSION_H

#include <vector>

#include "target_expression.h"

class CountExpression : public TargetExpression {
public:
    CountExpression(Expression *exp);

    ~CountExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override;
};



#endif //COUNT_EXPRESSION_H
