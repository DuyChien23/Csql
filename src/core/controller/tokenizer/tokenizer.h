//
// Created by chiendd on 24/09/2024.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>

#include "keywords.h"
#include "scanner.h"
#include "../../util/errors.h"

enum class TokenType {
    statement, identifier, keyword, number, operators, timedate, punctuation, string, function, unknown
};

struct Token {
    Token &operator=(const Token &aCopy) {
        type = aCopy.type;
        keyword = aCopy.keyword;
        data = aCopy.data;
        return *this;
    }

    bool operator==(const Token &aToken) const {
        return type == aToken.type && keyword == aToken.keyword && op == aToken.op && data == aToken.data;
    }

    TokenType type;
    SqlKeywords keyword;
    SqlOperators op;
    SqlFunctions function;
    std::string data;
};

class Tokenizer : public Scanner {
public:
    explicit Tokenizer(std::istream &anInputStream) : Scanner(anInputStream) {
    };

    void tokenize();

    std::string getStringUntil(std::string aData);

    [[nodiscard]] size_t size() const;

    Token *tokenAt(size_t anOffset);

    Token *peek();

    Token *get();

    Tokenizer *check(SqlKeywords aKeyword);

    Tokenizer *check(SqlOperators anOperator);

    Tokenizer *check(SqlFunctions aFt);

    Tokenizer *check(std::string aData);

    void checkOne();

    void endBy(std::string aData);

    Tokenizer *reset();

    bool consumeAttribute(std::string &tableName, std::string &attributeName);

    bool currentIs() { return false; };

    template<typename T, typename... Args>
    bool currentIs(const T firstData, const Args... restData) {
        bool result = currentIs(restData...);

        if constexpr (std::is_same_v<T, TokenType>) {
            return peek()->type == firstData || result;
        }
        if constexpr (std::is_same_v<T, SqlKeywords>) {
            if (peek()->type != TokenType::keyword) {
                return result;
            }
            return peek()->keyword == firstData || result;
        }
        if constexpr (std::is_same_v<T, SqlOperators>) {
            if (peek()->type != TokenType::operators) {
                return result;
            }
            return peek()->op == firstData || result;
        }
        if constexpr (std::is_same_v<T, SqlFunctions>) {
            if (peek()->type != TokenType::function) {
                return result;
            }
            return peek()->function == firstData || result;
        }
        if constexpr (std::is_same_v<T, int>) {
            if (peek()->type != TokenType::number) {
                return result;
            }
            return std::stoi(peek()->data) == firstData || result;
        }
        if constexpr (std::is_same_v<T, float>) {
            if (peek()->type != TokenType::number) {
                return result;
            }
            return std::stod(peek()->data) == firstData || result;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return peek()->data == firstData || result;
        }
        return result;
    }

    bool currentIsChar(char aChar) {
        if (peek()->type != TokenType::punctuation) {
            return false;
        }
        return peek()->data[0] == aChar;
    }

    template<typename T>
    Tokenizer *consumeType(T &aData) {
        if constexpr (std::is_same_v<T, int>) {
            if (peek()->type != TokenType::number) {
                throw Errors("Unexpected token: " + peek()->data);
            }
            aData = std::stoi(peek()->data);
        }
        if constexpr (std::is_same_v<T, SqlKeywords>) {
            if (peek()->type != TokenType::keyword) {
                throw Errors("Unexpected token: " + peek()->data);
            }
            aData = peek()->keyword;
        }
        if constexpr (std::is_same_v<T, SqlOperators>) {
            if (peek()->type != TokenType::operators) {
                throw Errors("Unexpected token: " + peek()->data);
            }
            aData = peek()->op;
        }
        if constexpr (std::is_same_v<T, SqlFunctions>) {
            if (peek()->type != TokenType::function) {
                throw Errors("Unexpected token: " + peek()->data);
            }
            aData = peek()->function;
        }
        if constexpr (std::is_same_v<T, float>) {
            if (peek()->type != TokenType::number) {
                throw Errors("Unexpected token: " + peek()->data);
            }
            aData = std::stod(peek()->data);
        }
        if constexpr (std::is_same_v<T, std::string>) {
            aData = peek()->data;
        }

        index++;
        return this;
    }

protected:
    std::vector<Token> tokens;
    size_t index = 0;
};


#endif //TOKENIZER_H
