//
// Created by chiendd on 24/12/2024.
//

#ifndef TARGETEXPRESSION_H
#define TARGETEXPRESSION_H
#include "../expression.h"

enum TargetTypes {
    normal,
    sum,
    avg,
    count,
    count_all,
    max,
    min,
    group_concat
};

class TargetExpression {
public:
    TargetExpression(Expression *exp, TargetTypes _type, std::string _tableName = "", std::string _attributeName = "") {
        expression = exp;
        type = _type;
        tableName = _tableName;
        attributeName = _attributeName;
    }

    virtual ~TargetExpression() = default;

    virtual SqlTypes apply(const std::vector<JoinedTuple> &tuples) = 0;

    std::string tableName;
    std::string attributeName;
    TargetTypes type;
protected:
    Expression *expression = nullptr;
};

#endif //TARGETEXPRESSION_H
