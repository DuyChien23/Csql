//
// Created by chiendd on 20/12/2024.
//

#ifndef UPDATE_STATEMENT_H
#define UPDATE_STATEMENT_H
#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/sql_query.h"
#include "../../../storage/database/database.h"

class UpdateStatement : public Statement {
public:
    UpdateStatement(UpdateQueryPtr &aUpdateQuery, std::ostream &output) : Statement(output) {
        updateQuery = std::move(aUpdateQuery);
    }

    void execute() override {
        DatabasePtr database = DatabaseController::getDatabase();
        database->update(output, updateQuery);
    }

private:
    UpdateQueryPtr updateQuery;
};

#endif //UPDATE_STATEMENT_H
