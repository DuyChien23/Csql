//
// Created by chiendd on 24/12/2024.
//

#ifndef COUNT_ALL_EXPRESSION_H
#define COUNT_ALL_EXPRESSION_H
#include "target_expression.h"


class CountAllExpression : public TargetExpression {
public:
    CountAllExpression() : TargetExpression(nullptr, count_all) {
    };

    ~CountAllExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override {
        return tuples.size();
    }
};



#endif //COUNT_ALL_EXPRESSION_H
