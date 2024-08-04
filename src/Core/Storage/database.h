//
// Created by chiendd on 21/07/2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include "IO_Manager/io_manager.h"
#include "../Util/config.h"

namespace Csql {
    class Database;

    class Database : IOManager {
    public:
        Database(std::string databaseName) : name(databaseName) {};

        ~Database() {};

        void std::string getPath() {
            return this -> path + this -> name + Config::databaseMetadataFileName;
        }

        const std::string path = "../../Databases/";
    protected:
        std::string name;


    };
}

#endif //DATABASE_H
