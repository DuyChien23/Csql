//
// Created by chiendd on 06/10/2024.
//

#ifndef CONTROLLER_INSTANCE_H
#define CONTROLLER_INSTANCE_H
#include <map>
#include <mutex>
#include <thread>

#include "../storage/database.h"
#include "../util/helpers.h"

namespace Csql {
    class DatabaseController {
    public:
        DatabaseController() = delete;

        static DatabasePtr getDatabase() {
            std::lock_guard lock(mtx);
            auto id = std::this_thread::get_id();
            return databases.contains(id) ? databases[id].get() : nullptr;
        }

        static void closeDatabase() {
            std::lock_guard lock(mtx);
            auto id = std::this_thread::get_id();
            if (!databases.contains(id)) return;
            databases[id].reset();
            databases.erase(id);
        }

        static void setDatabase(const std::string& aDatabaseName) {
            std::lock_guard lock(mtx);

            if (!Helpers::FolderHandle::containFolder(Configs::databaseDictionaryName, aDatabaseName)) {
                throw Errors("Database not exists");
            }

            auto id = std::this_thread::get_id();
            databases[id] = std::make_unique<Database>(aDatabaseName);
        }

    private:
        static std::mutex mtx;
        static std::map<std::thread::id, UniqueDatabasePtr> databases;
    };

}

#endif //CONTROLLER_INSTANCE_H
