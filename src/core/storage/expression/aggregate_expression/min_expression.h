//
// Created by chiendd on 24/12/2024.
//

#ifndef MIN_EXPRESSION_H
#define MIN_EXPRESSION_H
#include "target_expression.h"


class MinExpression : public TargetExpression {
public:
    MinExpression(Expression *exp) : TargetExpression(exp, min) {
    };

    ~MinExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override {
        SqlTypes result = expression->apply(tuples[0]);
        for (auto &tuple: tuples) {
            SqlTypes value = expression->apply(tuple);
            if (value < result) {
                result = value;
            }
        }
        return result;
    }
};



#endif //MIN_EXPRESSION_H
