//
// Created by chiendd on 11/09/2024.
//

#ifndef JOIN_EXPRESSION_H
#define JOIN_EXPRESSION_H
#include <string>

#include "expression.h"

enum class JoinTypes {
    inner,
    left,
    right,
    cross,
};

class JoinExpression {
public:
    JoinExpression(JoinTypes type, std::string _targetEntityName);

    JoinExpression(Expression *anExpression);

    [[nodiscard]] bool apply(const JoinedTuple &tuple) const;

    void addOrExpression(Expression *anExpression);

    void addAndExpression(Expression *anExpression);

    JoinTypes joinType;
    std::string targetEntityName;

protected:
    Expression *expression;
};

#endif //JOIN_EXPRESSION_H
