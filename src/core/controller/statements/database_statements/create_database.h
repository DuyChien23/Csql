//
// Created by chiendd on 06/10/2024.
//

#ifndef CREATE_DATABASE_H
#define CREATE_DATABASE_H

#include "../statement.h"
#include "../../database_controller.h"

namespace Csql {
    class CreateDatabaseStatement : public Statement {
    public:
        CreateDatabaseStatement(const std::string& databaseName, std::ostream& output) : Statement(output) {
            this->databaseName = databaseName;
        }
        void execute() override {
            try {
                if (Helpers::FolderHandle::createIfNotExist(databaseName, Configs::databaseDictionaryName)) {
                    output << "Database " << databaseName << " created" << std::endl;
                } else {
                    throw Errors("Database " + databaseName + " already exists");
                }
            } catch (Errors& e) {
                output << e.what() << std::endl;
            }
        }
    private:
        std::string databaseName;
    };
}

#endif //CREATE_DATABASE_H
