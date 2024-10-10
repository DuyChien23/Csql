//
// Created by chiendd on 06/10/2024.
//

#ifndef INSERT_STATEMENT_H
#define INSERT_STATEMENT_H

#include "../statement.h"
#include "../../database_controller.h"
#include "../../../storage/database.h"

namespace Csql {
    class InsertStatement : public Statement {
    public:
        InsertStatement(
            const std::string& aEntity,
            std::ostream& output,
            const std::vector<std::string>& aNames,
            const std::vector<std::string>& aValues
        ) : Statement(output) {
            entity = aEntity;
            names = aNames;
            values = aValues;
        }

        void execute() override {
            DatabasePtr database = DatabaseController::getDatabase();

            if (!names.empty() && names.size() > values.size()) {
                throw Errors("Number of attributes and values do not match");
            }

            std::vector<std::pair<std::string, std::string>> attributes(values.size());
            for (int i = 0; i < values.size(); ++i) {
                attributes[i].first = names.empty() ? "" : names[i];
                attributes[i].second = values[i];
            }
            database->insert(output, entity, attributes, !names.empty());
        }
    protected:
        std::string entity;
        std::vector<std::string> names;
        std::vector<std::string> values;
    };
}

#endif //INSERT_STATEMENT_H
