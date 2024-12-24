//
// Created by chiendd on 24/12/2024.
//

#ifndef AVG_EXPRESSION_H
#define AVG_EXPRESSION_H
#include "target_expression.h"

class AvgExpression : public TargetExpression {
public:
    AvgExpression(Expression *exp) : TargetExpression(exp, avg) {
    };

    ~AvgExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override;
};



#endif //AVG_EXPRESSION_H
