//
// Created by chiendd on 10/09/2024.
//

#ifndef WHERE_EXPRESSION_H
#define WHERE_EXPRESSION_H
#include "expression.h"

class WhereExpression {
public:
    explicit WhereExpression(Expression *_expression) : expression(_expression) {
    };

    WhereExpression();

    ~WhereExpression() = default;

    [[nodiscard]] bool apply(const JoinedTuple &tuple) const;

    void addOrExpression(Expression *anExpression);

    void addAndExpression(Expression *anExpression);

private:
    Expression *expression;
};


#endif //WHERE_EXPRESSION_H
