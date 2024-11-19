//
// Created by chiendd on 06/10/2024.
//

#ifndef SHOW_DATABASES_H
#define SHOW_DATABASES_H

#include "../statement.h"
#include "../../../view/table_view.h"

class ShowDatabasesStatement : public Statement {
public:
    ShowDatabasesStatement(std::ostream &output) : Statement(output) {
    }

    void execute() override {
        TableView theTableView("Databases");
        Helpers::FolderHandle::eachFolder(Configs::databaseDictionaryName, [&](const std::string &folderName) {
            theTableView.addData(folderName);
        });
        theTableView.show(output);
    }
};


#endif //SHOW_DATABASES_H
