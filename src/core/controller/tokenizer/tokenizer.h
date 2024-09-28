//
// Created by chiendd on 24/09/2024.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>

#include "keywords.h"
#include "scanner.h"

namespace Csql {
    enum class TokenType {
        statement, identifier, keyword, number, operators, timedate, punctuation, string, unknown
    };

    struct Token {
        Token& operator=(const Token& aCopy) {
            type = aCopy.type;
            keyword = aCopy.keyword;
            data = aCopy.data;
            return *this;
        }

        TokenType       type;
        SqlKeywords     keyword;
        SqlOperators    op;
        std::string     data;
    };

    class Tokenizer : public Scanner {
    public:
        Tokenizer(std::istream& anInputStream) : Scanner(anInputStream) {};

        void tokenize();
        size_t size() const;
        Token* tokenAt(size_t anOffset);

        inline Token *peek();
        Tokenizer *check(SqlKeywords aKeyword);

        template<typename T>
        Tokenizer *skipType(TokenType aType, T& aData);
        Tokenizer *skipData(std::string aData);

    protected:
        std::vector<Token> tokens;
        size_t index = 0;
    };
}


#endif //TOKENIZER_H
