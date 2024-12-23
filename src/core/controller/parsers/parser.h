//
// Created by chiendd on 26/09/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "../statements/statement.h"
#include "../tokenizer/tokenizer.h"

#define RETURN_IF_CONDITION_FALSE(condition) if (!(condition)) {return nullptr;}

class Parser {
public:
    explicit Parser(std::ostream &aOutput) : output(aOutput) {
    };

    virtual ~Parser() = default;

    virtual Statement *makeStatement(Tokenizer &aTokenizer) = 0;

protected:
    std::ostream &output;
};


#endif //PARSER_H
