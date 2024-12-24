//
// Created by chiendd on 24/12/2024.
//

#ifndef GROUP_CONCAT_EXPRESSION_H
#define GROUP_CONCAT_EXPRESSION_H
#include "target_expression.h"


class GroupConcatExpression : public TargetExpression {
public:
    GroupConcatExpression(Expression *exp, std::string _split = ",") : split(_split), TargetExpression(exp, group_concat) {
    };

    ~GroupConcatExpression() override = default;

    SqlTypes apply(const std::vector<JoinedTuple> &tuples) override;
private:
    std::string split;
};



#endif //GROUP_CONCAT_EXPRESSION_H
