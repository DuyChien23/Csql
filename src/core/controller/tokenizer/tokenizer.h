//
// Created by chiendd on 24/09/2024.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <vector>

#include "keywords.h"
#include "scanner.h"
#include "../../util/errors.h"

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
        explicit Tokenizer(std::istream& anInputStream) : Scanner(anInputStream) {};

        void tokenize();
        [[nodiscard]] size_t size() const;
        Token* tokenAt(size_t anOffset);

        inline Token *peek();
        bool check(SqlKeywords aKeyword);
        bool check(std::string aData);
        bool endBy(std::string aData);
        Tokenizer *reset();

        template<typename T>
        bool currentIs(const T aData) {
            if (!peek()) return false;
            if constexpr (std::is_same_v<T, SqlKeywords>) {
                if (peek()->type != TokenType::keyword) {
                    return false;
                }
                return mSqlKeywords[peek()->data] == aData;
            }
            if constexpr (std::is_same_v<T, int>) {
                if (peek()->type != TokenType::number) {
                    return false;
                }
                return std::stoi(peek()->data) == aData;
            }
            if constexpr (std::is_same_v<T, float>) {
                if (peek()->type != TokenType::number) {
                    return false;
                }
                return std::stod(peek()->data) == aData;
            }
            if constexpr (std::is_same_v<T, std::string>) {
                return peek()->data == aData;
            }
            return false;
        }

        bool currentIsChar(char aChar) {
            if (!peek()) return false;
            if (peek()->type != TokenType::punctuation) {
                return false;
            }
            return peek()->data[0] == aChar;
        }

        template<typename T>
        Tokenizer *skipType(T& aData) {
            if (!peek()) return nullptr;
            if constexpr (std::is_same_v<T, int>) {
                if (peek()->type != TokenType::number) {
                    return nullptr;
                }
                aData = std::stoi(peek()->data);
            }
            if constexpr (std::is_same_v<T, SqlKeywords>) {
                if (peek()->type != TokenType::keyword) {
                    return nullptr;
                }
                aData = mSqlKeywords[peek()->data];
            }
            if constexpr (std::is_same_v<T, float>) {
                if (peek()->type != TokenType::number) {
                    return nullptr;
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
}


#endif //TOKENIZER_H
