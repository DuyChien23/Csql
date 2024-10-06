//
// Created by chiendd on 06/10/2024.
//

#include "main_controller.h"

namespace Csql {
    MainController::MainController(std::ostream &aOutput) : output(aOutput) {
        parsers.push_back(new SqlParser(aOutput));
    }

    std::string MainController::getOutput() const {
        std::stringstream temp;
        temp << output.rdbuf();
        return temp.str();
    }

    Statement *MainController::parser(Tokenizer &aTokenizer) {
        for (auto parser : parsers) {
            Statement* statement = parser->makeStatement(aTokenizer);
            if (statement != nullptr) {
                return statement;
            }
        }
        return nullptr;
    }
}