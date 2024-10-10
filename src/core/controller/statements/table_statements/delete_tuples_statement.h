//
// Created by chiendd on 11/10/2024.
//

#ifndef DELETE_TUPLES_STATEMENT_H
#define DELETE_TUPLES_STATEMENT_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database.h"
#include "../../../storage/sql_query.h"

namespace Csql {
    class DeleteTuplesStatement : public Statement {
    public:
        DeleteTuplesStatement(SQLQueryPtr& aDeleteQuery, std::ostream& output) : Statement(output) {
            deleteQuery = std::move(aDeleteQuery);
        }

        void execute() override {
            DatabasePtr database = DatabaseController::getDatabase();
            database->deleteTuples(output, deleteQuery);
        }
    private:
        SQLQueryPtr deleteQuery;
    };
}

#endif //DELETE_TUPLES_STATEMENT_H
