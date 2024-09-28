//
// Created by chiendd on 22/07/2024.
//

#include <string>
#include "core/server/sql_server.h"
#include "core/util/configs.h"
#include "core/util/helpers.h"

int main(int argc, const char * argv[]) {
    std::string theServerAddress = Configs::defaultServerAddress;
    unsigned short theServerPort = Configs::defaultServerPort;

    if (argc == 3) {
        theServerAddress = argv[1];
        theServerPort = std::stoi(argv[2]);
    }

    Csql::Helpers::FolderHandle::createIfNotExist(Configs::databaseDictionaryName);

    Csql::SQLServer theServer(theServerAddress, theServerPort);
    theServer.run();
}

