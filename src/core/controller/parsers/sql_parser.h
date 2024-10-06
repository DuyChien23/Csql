//
// Created by chiendd on 26/09/2024.
//

#ifndef SQL_PARSER_H
#define SQL_PARSER_H
#include "parser.h"

namespace Csql {
    class SqlParser : public Parser {
        using SqlParserFactory = Statement*(SqlParser::*)(Tokenizer*);
    public:
        explicit SqlParser(std::ostream& output) : Parser(output) {};
        Statement* makeStatement(Tokenizer &aTokenizer) override;
    private:
        const std::vector<SqlParserFactory> factories = {
            &SqlParser::createEntityStatement,
        };

        Statement* createEntityStatement(Tokenizer *aTokenizer);
    };
}

#endif //SQL_PARSER_H