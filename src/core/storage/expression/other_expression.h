//
// Created by chiendd on 11/09/2024.
//

#ifndef BETWEEN_EXPRESSION_H
#define BETWEEN_EXPRESSION_H
#include <vector>

#include "expression.h"

enum class OtherOperator {
    between,
    not_between,
    in,
    not_in,
};

class OtherExpression : public Expression {
public:
    OtherExpression(OtherOperator _op, Expression *_value, std::vector<SqlTypes> _list);

    SqlTypes apply(const JoinedTuple &tuple) override;

private:
    OtherOperator op;
    Expression *value;
    std::vector<SqlTypes> list;
};


#endif //BETWEEN_EXPRESSION_H
