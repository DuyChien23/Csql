//
// Created by chiendd on 24/09/2024.
//

#include <cstring>

#include "tokenizer.h"

#include <iostream>

#include "../../storage/expression/other_expression.h"
#include "../../util/errors.h"

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
            if (theData[0] == '.' || theData[theData.size() - 1] == '.' || std::count(
                    theData.begin(), theData.end(), '.') > 1) {
                throw Errors("Invalid number");
            }
            tokens.push_back({
                TokenType::number, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft, theData
            });
        } else if (isQuote(theChar)) {
            char theQuote = input.get();
            std::string theData = readUntilChar(theQuote);
            tokens.push_back({
                TokenType::string, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft, theData
            });
            if (!skipIfChar(theQuote)) {
                throw Errors("Invalid quote");
            }
        } else if (isPunctuation(theChar)) {
            std::string theData = readOne();
            tokens.push_back({
                TokenType::punctuation, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft,
                theData
            });
        } else if (isOperator(theChar)) {
            std::string theData = read(isOperator);
            if (!mSqlOperators.contains(theData)) {
                throw Errors("Invalid operator");
            }
            tokens.push_back({
                TokenType::operators, SqlKeywords::unkonwn_kw, mSqlOperators[theData], SqlFunctions::unknown_ft, theData
            });
        } else {
            std::string theData = read(isAlphaNum);
            std::string theTemp(theData);
            std::transform(theTemp.begin(), theTemp.end(), theTemp.begin(), ::tolower);

            Token token{
                TokenType::keyword, SqlKeywords::unkonwn_kw, SqlOperators::unknown_op, SqlFunctions::unknown_ft, theTemp
            };

            if (mSqlKeywords.contains(theTemp)) {
                token.keyword = mSqlKeywords[theTemp];
            } else if (mSqlOperators.contains(theTemp)) {
                token.type = TokenType::operators;
                token.op = mSqlOperators[theTemp];
                token.data = theData;
            } else if (mSqlFunctions.contains(theTemp)) {
                token.type = TokenType::function;
                token.function = mSqlFunctions[theTemp];
            } else {
                token.type = TokenType::identifier;
                token.data = theData;
            }
            tokens.push_back(token);
        }
    }

    while (!tokens.empty() && tokens.back().data == "") {
        tokens.pop_back();
    }
}

size_t Tokenizer::size() const {
    return tokens.size();
}

Token *Tokenizer::tokenAt(size_t id) {
    return id < size() ? &tokens[id] : nullptr;
}

Token *Tokenizer::peek() {
    if (index >= size()) {
        throw Errors("Unexpected end of statement");
    }
    return index < size() ? &tokens[index] : nullptr;
}

Token *Tokenizer::get() {
    Token *result = peek();
    checkOne();
    return result;
}

Tokenizer *Tokenizer::check(SqlKeywords aKeyword) {
    if (peek()->type != TokenType::keyword) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    if (peek()->keyword != aKeyword) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    index++;

    return this;
}

Tokenizer *Tokenizer::check(SqlOperators anOperator) {
    if (peek()->type != TokenType::operators) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    if (peek()->op != anOperator) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    index++;

    return this;
}

Tokenizer *Tokenizer::check(SqlFunctions aFt) {
    if (peek()->type != TokenType::function) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    if (peek()->function != aFt) {
        throw Errors("Unexpected token: " + peek()->data);
    }
    index++;

    return this;
}

Tokenizer *Tokenizer::check(std::string aData) {
    if (peek()->data != aData) {
        throw Errors("Unexpected token: " + peek()->data);
    }

    index++;
    return this;
}

void Tokenizer::endBy(std::string aData) {
    check(aData);
    if (index < size()) {
        throw Errors("Unexpected token: " + peek()->data);
    }
}

Tokenizer *Tokenizer::reset() {
    index = 0;
    return this;
}

bool Tokenizer::consumeAttribute(std::string &tableName, std::string &attributeName) {
    if (peek()->type != TokenType::identifier) {
        return false;
    }
    consumeType(tableName);
    if (peek()->data == ".") {
        checkOne();
        consumeType(attributeName);
    } else {
        attributeName = tableName;
        tableName = "";
    }
    return true;
}

void Tokenizer::checkOne() {
    peek();
    index++;
}
