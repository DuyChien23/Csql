//
// Created by chiendd on 28/09/2024.
//

#ifndef CREATE_STATEMENT_H
#define CREATE_STATEMENT_H
#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database.h"
#include "../../../storage/entity.h"

namespace Csql {
    class CreateTableStatement : public Statement {
    public:
        CreateTableStatement(SharedEntityPtr& aEntity, std::ostream& output) : Statement(output) {
            entity = std::move(aEntity);
        }

        void execute() override {
            DatabasePtr database = DatabaseController::getDatabase();
            database->createTable(output, entity);
        }
    protected:
        SharedEntityPtr entity;
    };
}

#endif //CREATE_STATEMENT_H
