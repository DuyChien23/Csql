//
// Created by chiendd on 10/09/2024.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "../../util/types.h"

class Expression {
public:
    virtual SqlTypes apply(const JoinedTuple &tuple) = 0;

    virtual ~Expression() = default;
};


#endif //EXPRESSION_H
