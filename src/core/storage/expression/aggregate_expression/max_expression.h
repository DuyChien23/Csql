//
// Created by chiendd on 24/12/2024.
//

#ifndef MAX_EXPRESSION_H
#define MAX_EXPRESSION_H
#include "target_expression.h"


class MaxExpression : public TargetExpression {
public:
    MaxExpression(Expression *exp) : TargetExpression(exp, max) {
    };

    ~MaxExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override {
        SqlTypes result = expression->apply(tuples[0]);
        for (auto &tuple: tuples) {
            SqlTypes value = expression->apply(tuple);
            if (value > result) {
                result = value;
            }
        }
        return result;
    }
};

#endif //MAX_EXPRESSION_H
