//
// Created by chiendd on 06/10/2024.
//

#ifndef DROP_ENTITY_STATEMENT_H
#define DROP_ENTITY_STATEMENT_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database.h"

namespace Csql {
    class DropTableStatement : public Statement {
    public:
        DropTableStatement(const std::string& entityName, std::ostream& output) : Statement(output) {
            this->entityName = entityName;
        }

        void execute() override {
            DatabasePtr database = DatabaseController::getDatabase();
            database->dropTable(output, entityName);
        }
    protected:
        std::string entityName;
    };
}

#endif //DROP_ENTITY_STATEMENT_H
