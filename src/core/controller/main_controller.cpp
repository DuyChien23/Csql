//
// Created by chiendd on 06/10/2024.
//

#include "main_controller.h"

#include "parsers/database_parser.h"
#include "parsers/sql_parser.h"
#include "parsers/txn_parser.h"

MainController::MainController(std::ostream &aOutput) : output(aOutput) {
    parsers.push_back(new SqlParser(aOutput));
    parsers.push_back(new DatabaseParser(aOutput));
    parsers.push_back(new TxnParser(aOutput));
}

std::string MainController::getOutput() const {
    std::stringstream temp;
    temp << output.rdbuf();
    return temp.str();
}

Statement *MainController::parser(Tokenizer &aTokenizer) {
    for (auto parser: parsers) {
        Statement *statement = parser->makeStatement(aTokenizer);
        if (statement != nullptr) {
            return statement;
        }
    }
    return nullptr;
}
