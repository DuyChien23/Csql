//
// Created by chiendd on 06/10/2024.
//

#ifndef DESCRIBE_TABLE_STATEMENT_H
#define DESCRIBE_TABLE_STATEMENT_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database/database.h"

class DescribeTableStatement : public Statement {
public:
    DescribeTableStatement(const std::string &entityName, std::ostream &output) : Statement(output) {
        this->entityName = entityName;
    }

    void execute() override {
        DatabasePtr database = DatabaseController::getDatabase();
        database->describeTable(output, entityName);
    }

protected:
    std::string entityName;
};


#endif //DESCRIBE_TABLE_STATEMENT_H
