//
// Created by chiendd on 22/12/2024.
//

#ifndef FUNCTION_EXPRESSION_H
#define FUNCTION_EXPRESSION_H
#include "expression.h"

enum class FunctionTypes {
    ascii_ft, concat_ft, concat_ws_ft, char_length_ft, char_ft, length_ft, left_ft, locate_ft, lower_ft, substring_ft,
    substr_ft, replace_ft, right_ft, reverse_ft, trim_ft, upper_ft,
};

class FunctionExpression : public Expression {
public:
    FunctionExpression(FunctionTypes aType, std::vector<Expression *> &theParameters);

    SqlTypes apply(const JoinedTuple &tuple) override;
private:
    FunctionTypes type;
    std::vector<Expression *> parameters;
};



#endif //FUNCTION_EXPRESSION_H
