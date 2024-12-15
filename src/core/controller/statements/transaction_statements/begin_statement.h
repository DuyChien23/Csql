//
// Created by chiendd on 20/12/2024.
//

#ifndef BEGIN_STATEMENT_H
#define BEGIN_STATEMENT_H
#include "../statement.h"
#include "../../database_controller.h"

#endif //BEGIN_STATEMENT_H


class BeginStatement : public Statement {
public:
    BeginStatement(std::ostream &output) : Statement(output) {};

    void execute() override {
        auto theDatabase = DatabaseController::getDatabase();
        theDatabase->beginTransaction();
    }
};
