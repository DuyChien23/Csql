//
// Created by chiendd on 06/10/2024.
//

#ifndef SHOW_TABLES_H
#define SHOW_TABLES_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database/database.h"

class ShowTablesStatement : public Statement {
public:
    ShowTablesStatement(std::ostream &output) : Statement(output) {
    }

    void execute() override {
        DatabasePtr database = DatabaseController::getDatabase();
        database->showTables(output);
    }
};


#endif //SHOW_TABLES_H
