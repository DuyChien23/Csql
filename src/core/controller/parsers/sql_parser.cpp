//
// Created by chiendd on 26/09/2024.
//

#include "sql_parser.h"

namespace Csql {
    Statement* SqlParser::makeStatement(Tokenizer &aTokenizer) {
        for (auto &theFactory : factories) {
            Statement *theStatement = theFactory(&aTokenizer);
            if (theStatement != nullptr) {
                return theStatement;
            }
        }
        return nullptr;
    }

    Statement* SqlParser::createEntityStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::create_kw)->check(SqlKeywords::table_kw));
        RETURN_IF_CONDITION_FALSE(aTokenizer->peek());

        std::string theEntityName;
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(TokenType::identifier, theEntityName));
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));

        while (aTokenizer->peek()) {
            RETURN_IF_CONDITION_FALSE(aTokenizer->peek());
            std::string attributeName;
            RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(TokenType::identifier, attributeName));
            std::string type;
            RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(TokenType::identifier, type));
            RETURN_IF_CONDITION_FALSE(mSqlKeywords.contains(type));
            switch (aTokenizer->peek()->data) {
                case "varchar":
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipData("("));
                    int size;
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(TokenType::number, size));
                    RETURN_IF_CONDITION_FALSE(aTokenizer->skipData(")"));
                    break;
                case "int":
                    break;
                case "float":
                    break;
                case "datetime":
                    break;
                case "bool":
                    break;
                default:
                    return nullptr;
            }
        }


        return nullptr;
    }
}
