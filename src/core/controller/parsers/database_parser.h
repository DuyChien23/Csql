//
// Created by chiendd on 06/10/2024.
//

#ifndef DATABASE_PARSER_H
#define DATABASE_PARSER_H
#include "parser.h"
#include "../../storage/database/database.h"

class DatabaseParser : public Parser {
    using DatabaseParserFactory = Statement*(DatabaseParser::*)(Tokenizer *);

public:
    DatabaseParser(std::ostream &output) : Parser(output) {
    };

    Statement *makeStatement(Tokenizer &aTokenizer) override;

private:
    std::vector<DatabaseParserFactory> factories = {
        &DatabaseParser::useDatabaseStatement,
        &DatabaseParser::createDatabaseStatement,
        &DatabaseParser::showTableStatement,
        &DatabaseParser::showDatabasesStatement,
        &DatabaseParser::dropDatabaseStatement
    };

    Statement *useDatabaseStatement(Tokenizer *aTokenizer);

    Statement *createDatabaseStatement(Tokenizer *aTokenizer);

    Statement *showTableStatement(Tokenizer *aTokenizer);

    Statement *showDatabasesStatement(Tokenizer *aTokenizer);

    Statement *dropDatabaseStatement(Tokenizer *aTokenizer);
};


#endif //DATABASE_PARSER_H
