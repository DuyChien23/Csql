//
// Created by chiendd on 24/09/2024.
//

#include <cstring>

#include "tokenizer.h"

#include <iostream>

#include "../../storage/expression/other_expression.h"
#include "../../util/errors.h"

namespace Csql {
    bool isWhitespace(char theChar) {
        return strchr(" \t\n\r\b", theChar);
    }

    bool isNumber(char theChar) {
        return isdigit(theChar) || '.' == theChar;
    }

    bool isQuote(char theChar) {
        return '\'' == theChar || '\"' == theChar;
    }

    bool isOperator(char theChar) {
        return strchr("+-/*%=>!<.", theChar);
    }

    bool isPunctuation(char theChar) {
        return strchr("()[]{}:;,|", theChar);
    }

    bool isAlphaNum(char theChar) {
        return isalnum(theChar) || '_' == theChar;
    }

    void Tokenizer::tokenize() {
        while (!eof()) {
            char theChar = input.peek();
            if (isWhitespace(theChar)) {
                skip(isWhitespace);
            } else if (isdigit(theChar)) {
                std::string theData = read(isNumber);
                if (theData[0] == '.' || theData[theData.size() - 1] == '.' || std::count(theData.begin(), theData.end(), '.') > 1) {
                    throw Errors("Invalid number");
                }
                tokens.push_back({TokenType::number, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, theData});
            } else if (isQuote(theChar)) {
                char theQuote = input.get();
                std::string theData = readUntilChar(theQuote);
                tokens.push_back({TokenType::string, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, theData});
                if (!skipIfChar(theQuote)) {
                    throw Errors("Invalid quote");
                }
            } else if (isOperator(theChar)) {
                std::string theData = read(isOperator);
                if (!mSqlOperators.contains(theData)) {
                    throw Errors("Invalid operator");
                }
                tokens.push_back({TokenType::operators, SqlKeywords::unkonwn_kw, mSqlOperators[theData], theData});
            } else if (isPunctuation(theChar)) {
                std::string theData = readOne();
                tokens.push_back({TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, theData});
            } else {
                std::string theData = read(isAlphaNum);
                std::string theTemp(theData);
                std::transform(theTemp.begin(), theTemp.end(), theTemp.begin(), ::tolower);

                Token token{TokenType::keyword, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, theTemp};

                if (mSqlKeywords.contains(theTemp)) {
                    token.keyword = mSqlKeywords[theTemp];
                    tokens.push_back(token);
                } else {
                    token.type = TokenType::identifier;
                    token.data = theData;
                    tokens.push_back(token);
                }
            }
        }
    }

    size_t Tokenizer::size() const {
        return tokens.size();
    }

    Token* Tokenizer::tokenAt(size_t id) {
        return id < size() ? &tokens[id] : nullptr;
    }

    Token *Tokenizer::peek() {
        return index < size() ? &tokens[index] : nullptr;
    }

    Tokenizer *Tokenizer::check(SqlKeywords aKeyword) {
        if (!peek()) {
            return nullptr;
        }
        if (peek()->type != TokenType::keyword) {
            return nullptr;
        }
        if (!mSqlKeywords.contains(peek()->data) || mSqlKeywords[peek()->data] != aKeyword) {
            return nullptr;
        }
        index++;

        return this;
    }

    Tokenizer *Tokenizer::skipData(std::string aData) {
        if (!peek() || peek()->data != aData) {
            return nullptr;
        }
        index++;
        return this;
    }

    Tokenizer *Tokenizer::skipKeyword(SqlKeywords aKeyword) {
        if (!peek() || peek()->type != TokenType::keyword) {
            return nullptr;
        }
        if (!mSqlKeywords.contains(peek()->data) || mSqlKeywords[peek()->data] != aKeyword) {
            return nullptr;
        }
        index++;
        return this;
    }

    Tokenizer *Tokenizer::reset() {
        index = 0;
        return this;
    }

}
