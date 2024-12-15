//
// Created by chiendd on 19/12/2024.
//

#include "txn_parser.h"

#include "../statements/transaction_statements/abort_statement.h"
#include "../statements/transaction_statements/begin_statement.h"
#include "../statements/transaction_statements/commit_statement.h"

Statement *TxnParser::makeStatement(Tokenizer &aTokenizer) {
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

Statement *TxnParser::abortStatement(Tokenizer *aTokenizer) {
    aTokenizer->check(SqlKeywords::abort_kw)->endBy(";");
    return new AbortStatement(output);
}

Statement *TxnParser::commitStatement(Tokenizer *aTokenizer) {
    aTokenizer->check(SqlKeywords::commit_kw)->endBy(";");
    return new CommitStatement(output);
}

Statement *TxnParser::beginStatement(Tokenizer *aTokenizer) {
    aTokenizer->check(SqlKeywords::begin_kw)->check(SqlKeywords::transaction_kw)->endBy(";");
    return new BeginStatement(output);
}
