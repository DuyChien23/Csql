//
// Created by chiendd on 07/10/2024.
//

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database.h"
#include "../../../storage/sql_query.h"

namespace Csql {
    class SelectStatement : public Statement {
    public:
        SelectStatement(SQLQueryPtr& aSelectQuery, std::ostream& output) : Statement(output) {
            selectQuery = std::move(aSelectQuery);
        }

        void execute() override {
            DatabasePtr database = DatabaseController::getDatabase();
            database->select(output, selectQuery);
        }
    protected:
        SQLQueryPtr selectQuery;
    };
}

#endif //SELECT_STATEMENT_H
