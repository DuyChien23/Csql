//
// Created by chiendd on 12/08/2024.
//

#ifndef STORAGE_H
#define STORAGE_H
#include <filesystem>

#include "lru_cache/lru_cache.h"

namespace fs = std::filesystem;

namespace Csql {
    using EntityVisitor = std::function<void(SharedEntityPtr anEntity)>;
    using PageVisitor = std::function<void(SharedPagePtr aPage)>;

    class Storage {
    public:
        Storage(std::string databaseName) :
            pageCache(std::make_shared<LRUCache<std::string, SharedPagePtr>>(Configs::cacheMaxSize)),
            fullPath(Configs::databaseDictionaryName + "/" + databaseName),
            name(databaseName) {};

    protected:
        std::shared_ptr<LRUCache<std::string, SharedPagePtr>> pageCache;

        std::string fullPath;

        std::string name;

        std::map<std::string, std::fstream> mFstream;

        std::map<std::string, SharedEntityPtr> mEntity;

        std::vector<std::string> *listEntityNames = nullptr;

        std::vector<std::string>* getListEntityNames() {
            if (listEntityNames) {
                return listEntityNames;
            }

            auto trimFileExtension = [&](std::string s) {
                std::size_t pos = s.find_last_of(".");
                if (pos != std::string::npos) {
                    s.erase(pos);
                }
                return s;
            };

            listEntityNames = new std::vector<std::string>();
            for (const auto &entityFile : fs::directory_iterator(fullPath)) {
                if (fs::is_regular_file(entityFile.status())) {
                    listEntityNames->push_back(trimFileExtension(entityFile.path().filename().string()));
                }
            }

            return listEntityNames;
        }

        std::fstream& getFstream(std::string entityName) {
            if (!mFstream.contains(entityName)) {
                mFstream[entityName] = std::fstream(createEntityPath(entityName), MODIFYING_DISK_MODE);
            }
            return mFstream[entityName];
        }

        SharedEntityPtr getEntity(std::string entityName) {
            if (!std::filesystem::exists(createEntityPath(entityName))) {
                return nullptr;
            }

            if (!mEntity.contains(entityName)) {
                std::fstream &f = getFstream(entityName);
                f.seekg(0);
                SharedEntityPtr theEntity = std::make_shared<Entity>(this->name, entityName);
                theEntity->decode(f);
                theEntity->refresh();
                mEntity[entityName] = theEntity;
            }
            return mEntity[entityName];
        }

        std::string createKey(std::string entityName, uint32_t pageIndex) {
            return entityName + '#' + std::to_string(pageIndex);
        }

        std::string createEntityPath(std::string entityName) {
            return fullPath + "/" + entityName + Configs::entityFileExtension;
        }

        void writePage(SharedPagePtr aPage);
        void writePageUncache(SharedPagePtr aPage);

        void readPage(const std::string& entityName, uint32_t pageIndex, SharedPagePtr aPage);
        void readPageUncache(const std::string& entityName, uint32_t pageIndex, SharedPagePtr aPage);

        void deleteEntityFile(std::string entityName);
        void eachEntity(const EntityVisitor& entityVisitor);

        SharedPagePtr popFreePage(const std::string& entityName);
        void pushDataPage(const std::string &entityName, SharedPagePtr aPage);
        void eachDataPage(const std::string& entityName, const PageVisitor& pageVisitor);
    };
}



#endif //STORAGE_H
