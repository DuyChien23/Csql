//
// Created by chiendd on 06/10/2024.
//

#include "database_parser.h"

#include "../statements/database_statements/create_database.h"
#include "../statements/database_statements/show_tables.h"
#include "../statements/database_statements/show_databases.h"
#include "../statements/database_statements/use_database.h"

namespace Csql {
    Statement* DatabaseParser::makeStatement(Tokenizer &aTokenizer) {
        Statement *statement = nullptr;
        for (auto &factory : factories) {
            statement = (this->*factory)(aTokenizer.reset());
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

    Statement *DatabaseParser::createDatabaseStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::create_kw));
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::database_kw));
        std::string databaseName;
        RETURN_IF_CONDITION_FALSE(aTokenizer->skipType(databaseName));
        RETURN_IF_CONDITION_FALSE(aTokenizer->endBy(";"));

        return new CreateDatabaseStatement(databaseName, output);
    }

    Statement *DatabaseParser::showTableStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::show_kw));
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::tables_kw));

        return new ShowTablesStatement(output);
    }

    Statement *DatabaseParser::showDatabasesStatement(Tokenizer *aTokenizer) {
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::show_kw));
        RETURN_IF_CONDITION_FALSE(aTokenizer->check(SqlKeywords::databases_kw));

        return new ShowDatabasesStatement(output);
    }

}
