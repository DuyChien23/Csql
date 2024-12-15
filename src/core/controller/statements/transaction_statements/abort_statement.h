//
// Created by chiendd on 19/12/2024.
//

#ifndef ABORT_STATEMENT_H
#define ABORT_STATEMENT_H
#include <ostream>

#include "../../database_controller.h"

#endif //ABORT_STATEMENT_H

class AbortStatement : public Statement {
public:
    AbortStatement(std::ostream &output) : Statement(output) {};

    void execute() override {
        auto theDatabase = DatabaseController::getDatabase();
        theDatabase->abort();
    }
};