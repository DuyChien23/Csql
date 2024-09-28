//
// Created by chiendd on 26/09/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "../statements/statement.h"
#include "../tokenizer/tokenizer.h"

#define RETURN_IF_CONDITION_FALSE(condition) if (!(condition)) {return nullptr;}

namespace Csql {
    class Parser {
    public:
        virtual ~Parser() = default;
        virtual Statement* makeStatement(Csql::Tokenizer &aTokenizer) = 0;
    protected:
        Parser(std::ostream &anOutput) : output(anOutput) {}
        std::ostream &output;
    };
}

#endif //PARSER_H
