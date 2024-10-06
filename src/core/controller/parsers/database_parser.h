//
// Created by chiendd on 06/10/2024.
//

#ifndef DATABASE_PARSER_H
#define DATABASE_PARSER_H
#include "parser.h"
#include "../../storage/database.h"

namespace Csql {
    class DatabaseParser : public Parser {
        using DatabaseParserFactory = Statement*(DatabaseParser::*)(Tokenizer*);
    public:
        DatabaseParser(std::ostream& output) : Parser(output) {};
        Statement* makeStatement(Tokenizer &aTokenizer) override;
    private:
        std::vector<DatabaseParserFactory> factories = {
            &DatabaseParser::useDatabaseStatement,
        };

        Statement* useDatabaseStatement(Tokenizer *aTokenizer);
    };
}



#endif //DATABASE_PARSER_H
