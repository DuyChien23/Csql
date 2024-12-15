//
// Created by chiendd on 22/12/2024.
//

#ifndef SET_EXPRESSION_H
#define SET_EXPRESSION_H
#include "expression.h"

enum class SetTypes {
    range,
    list,
};

class SetExpression : public Expression {
public:
    SetExpression(SetTypes _type, std::vector<SqlTypes> _list);

    SqlTypes apply(const JoinedTuple &tuple) override;

private:
    std::vector<SqlTypes> list;
    SetTypes type;
};



#endif //SET_EXPRESSION_H
