//
// Created by chiendd on 06/10/2024.
//

#ifndef USE_DATABASE_H
#define USE_DATABASE_H

#include "../statement.h"
#include "../../database_controller.h"

#endif //USE_DATABASE_H

namespace Csql {
    class UseDatabaseStatement : public Statement {
    public:
        UseDatabaseStatement(const std::string& databaseName, std::ostream& output) : Statement(output) {
            this->databaseName = databaseName;
        }
        void execute() override {
            try {
                DatabaseController::setDatabase(databaseName);
                output << "Use database " << databaseName << std::endl;
            } catch (Errors& e) {
                output << e.what() << std::endl;
            }
        }
    private:
        std::string databaseName;
    };
}