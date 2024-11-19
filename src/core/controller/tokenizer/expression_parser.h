//
// Created by chiendd on 08/10/2024.
//

#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include "tokenizer.h"
#include "../../storage/expression/expression.h"

class ExpressionParser {
public:
    explicit ExpressionParser(Tokenizer *aTokenizer) : tokenizer(aTokenizer) {
    };

    Expression *parse();

private:
    Tokenizer *tokenizer;

    Expression *parseExpression(bool isLogicExpression = true);

    Expression *parseTerm(bool isLogicExpression = true);

    Expression *parseFactor(bool isLogicExpression = true);

    Expression *parseValue();

    [[nodiscard]] bool isContinued() const {
        if (tokenizer->currentIs(TokenType::identifier, TokenType::number, TokenType::operators, TokenType::string,
                                 TokenType::timedate)) {
            return true;
        }

        if (*tokenizer->peek() == Token{TokenType::keyword, SqlKeywords::and_kw, SqlOperators::unknown_op, "and"}) {
            return true;
        }
        if (*tokenizer->peek() == Token{TokenType::keyword, SqlKeywords::or_kw, SqlOperators::unknown_op, "or"}) {
            return true;
        }
        if (*tokenizer->peek() == Token{TokenType::keyword, SqlKeywords::not_kw, SqlOperators::unknown_op, "not"}) {
            return true;
        }
        if (*tokenizer->peek() == Token{
                TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, "("
            }) { return true; }
        if (*tokenizer->peek() == Token{
                TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, ")"
            }) { return true; }


        return false;
    }
};

#endif //EXPRESSION_PARSER_H
