//
// Created by chiendd on 06/10/2024.
//

#include "database_parser.h"

#include <iostream>

#include "../statements/database_statements/create_database.h"
#include "../statements/database_statements/show_tables.h"
#include "../statements/database_statements/show_databases.h"
#include "../statements/database_statements/use_database.h"

Statement *DatabaseParser::makeStatement(Tokenizer &aTokenizer) {
    for (auto &factory: factories) {
        try {
            Statement *statement = (this->*factory)(aTokenizer.reset());
            if (statement != nullptr) {
                return statement;
            }
        } catch (...) {
            // do nothing because we want to try the next factory
        }
    }
    return nullptr;
}

Statement *DatabaseParser::useDatabaseStatement(Tokenizer *aTokenizer) {
    std::string databaseName;
    aTokenizer->check(SqlKeywords::use_kw)->consumeType(databaseName)->endBy(";");
    return new UseDatabaseStatement(databaseName, output);
}

Statement *DatabaseParser::createDatabaseStatement(Tokenizer *aTokenizer) {
    std::string databaseName;
    aTokenizer->check(SqlKeywords::create_kw)->check(SqlKeywords::database_kw)->consumeType(databaseName)->endBy(";");

    return new CreateDatabaseStatement(databaseName, output);
}

Statement *DatabaseParser::showTableStatement(Tokenizer *aTokenizer) {
    aTokenizer->check(SqlKeywords::show_kw)->check(SqlKeywords::tables_kw)->endBy(";");

    return new ShowTablesStatement(output);
}

Statement *DatabaseParser::showDatabasesStatement(Tokenizer *aTokenizer) {
    aTokenizer->check(SqlKeywords::show_kw)->check(SqlKeywords::databases_kw)->endBy(";");

    return new ShowDatabasesStatement(output);
}
