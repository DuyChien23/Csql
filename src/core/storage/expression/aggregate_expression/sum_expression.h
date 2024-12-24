//
// Created by chiendd on 24/12/2024.
//

#ifndef SUM_EXPRESSION_H
#define SUM_EXPRESSION_H
#include "target_expression.h"

class SumExpression : public TargetExpression {
public:
    SumExpression(Expression *exp) : TargetExpression(exp, sum) {
    };

    ~SumExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override;
};



#endif //SUM_EXPRESSION_H
