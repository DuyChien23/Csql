//
// Created by chiendd on 06/10/2024.
//

#include "database_parser.h"

#include "../statements/database_statements/use_database.h"

namespace Csql {
    Statement* DatabaseParser::makeStatement(Tokenizer &aTokenizer) {
        Statement *statement = nullptr;
        for (auto &factory : factories) {
            statement = (this->*factory)(&aTokenizer);
            if (statement != nullptr) {
                break;
            }
        }
        return statement;
    }

    Statement* DatabaseParser::useDatabaseStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::use_kw));
        std::string databaseName;
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(databaseName));
        RETURN_IF_CONDITION_FALSE(aTokenizer->endBy(";"));

        return new UseDatabaseStatement(databaseName, output);
    }
}
