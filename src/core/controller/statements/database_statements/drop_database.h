//
// Created by chiendd on 28/11/2024.
//

#ifndef DROP_DATABASE_H
#define DROP_DATABASE_H
#include <string>

#include "../statement.h"
#include "../../database_controller.h"

class Errors;

class DropDatabaseStatement : public Statement {
public:
    DropDatabaseStatement(const std::string &databaseName, std::ostream &output) : Statement(output) {
        this->databaseName = databaseName;
    }

    void execute() override {
        try {
            if (Helpers::FolderHandle::deleteIfExist(databaseName, Configs::databaseDictionaryName)) {
                output << "Database " << databaseName << " deleted" << std::endl;
            } else {
                throw Errors("Database " + databaseName + " does not exist");
            }
        } catch (Errors &e) {
            output << e.what() << std::endl;
        }
    }

private:
    std::string databaseName;
};

#endif //DROP_DATABASE_H
