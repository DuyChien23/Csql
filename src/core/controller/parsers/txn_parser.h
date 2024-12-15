//
// Created by chiendd on 19/12/2024.
//

#ifndef TXN_PARSER_H
#define TXN_PARSER_H
#include "parser.h"


class TxnParser : public Parser {
public:
    using TxnParserFactory = Statement*(TxnParser::*)(Tokenizer *);

    explicit TxnParser(std::ostream &output) : Parser(output) {
    };

    Statement *makeStatement(Tokenizer &aTokenizer) override;

    const std::vector<TxnParserFactory> factories = {
        &TxnParser::abortStatement,
        &TxnParser::commitStatement,
        &TxnParser::beginStatement
    };

    Statement *abortStatement(Tokenizer *aTokenizer);
    Statement *commitStatement(Tokenizer *aTokenizer);
    Statement *beginStatement(Tokenizer *aTokenizer);
};



#endif //TXN_PARSER_H
