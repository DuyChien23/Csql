//
// Created by chiendd on 08/10/2024.
//

#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "tokenizer.h"
#include "../../storage/expression/arithmetic_expresstion.h"
#include "../../storage/expression/binary_expression.h"
#include "../../storage/expression/expression.h"
#include "../../storage/expression/function_expression.h"
#include "../../storage/expression/other_expression.h"
#include "../../storage/expression/unary_expression.h"

class ExpressionParser {
public:
    explicit ExpressionParser(Tokenizer *aTokenizer) : tokenizer(aTokenizer) {
    };

    Expression *parse(std::string endToken = "");

private:
    Tokenizer *tokenizer;

    [[nodiscard]] bool isContinued(std::string endToken) const {
        if (tokenizer->peek()->data == endToken) {
            return false;
        }
        if (tokenizer->currentIs(TokenType::identifier, TokenType::number, TokenType::operators, TokenType::string,
                                 TokenType::timedate, TokenType::function)) {
            return true;
        }

        if (*tokenizer->peek() == Token{
                TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft, "("
            }) { return true; }
        if (*tokenizer->peek() == Token{
                TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft, ")"
            }) { return true; }


        return false;
    }

    static std::map<SqlOperators, UnaryOperator> unaryMapping;
    static std::map<SqlOperators, BinaryOperator> binaryMapping;
    static std::map<SqlOperators, ArithmeticOperator> arithmeticMapping;
    static std::map<SqlOperators, OtherOperator> otherMapping;

    static std::map<SqlFunctions, FunctionTypes> functionMapping;
};

#endif //EXPRESSION_PARSER_H
