//
// Created by chiendd on 19/12/2024.
//

#ifndef COMMIT_STATEMENT_H
#define COMMIT_STATEMENT_H
#include <ostream>

#endif //COMMIT_STATEMENT_H

class CommitStatement : public Statement {
public:
    CommitStatement(std::ostream &output) : Statement(output) {};

    void execute() override {
        auto theDatabase = DatabaseController::getDatabase();
        theDatabase->commit();
    }
};