//
// Created by chiendd on 12/08/2024.
//

#ifndef STORAGE_H
#define STORAGE_H
#include <cassert>
#include <filesystem>
#include <iostream>

#include "lru_cache/lru_cache.h"

namespace fs = std::filesystem;

namespace Csql {
    using EntityVisitor = std::function<void(SharedEntityPtr anEntity)>;
    using PageVisitor = std::function<void(SharedPagePtr& aPage)>;

    using TupleVisitor = std::function<void(Tuple& tuple)>;

    using SplitNodeType = std::tuple<BPlusKey, SharedPagePtr, SharedPagePtr>;

    struct BtreeLeafIterator {
        int index;
        SharedPagePtr page;

        BtreeLeafIterator(int _index, SharedPagePtr _page) {
            assert(_page == nullptr || _page->pageType == PageType::leaf_b_plus_node);
            index = _index;
            page = std::move(_page);
        };

        bool operator!=(const BtreeLeafIterator& other) const {
            if (index != other.index) return true;
            if (index == -1) return false;
            return page->pageIndex != other.page->pageIndex;
        }

        bool operator==(const BtreeLeafIterator& other) const {
            return !(*this != other);
        }

        Tuple& operator*() {
            return page->getTuple(index);
        }
    };

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

        void saveEntity(const SharedEntityPtr& theEntity) {
            std::fstream &f = getFstream(theEntity->getName());
            f.seekg(0);
            theEntity->save();
            theEntity->encode(f);
        }

        void saveEntity(const std::string& entityName) {
            assert(mEntity.contains(entityName));
            saveEntity(getEntity(entityName));
        }

        std::string createKey(std::string entityName, uint32_t pageIndex) {
            return entityName + '#' + std::to_string(pageIndex);
        }

        std::string createEntityPath(std::string entityName) {
            return fullPath + "/" + entityName + Configs::entityFileExtension;
        }

        void writePage(SharedPagePtr &aPage);
        void writePageUncache(SharedPagePtr &aPage);

        void readPage(const std::string& entityName, uint32_t pageIndex, SharedPagePtr &aPage);
        void readPageUncache(const std::string& entityName, uint32_t pageIndex, SharedPagePtr &aPage);

        void deleteEntityFile(const std::string& entityName);
        void eachEntity(const EntityVisitor& entityVisitor);

        SharedPagePtr popFreePage(const std::string& entityName);
        SharedPagePtr addPageIntoFreeZone(SharedPagePtr &aPage);

        //===========================================B+Tree=Interface=========================================
        void setBTree(IndexingMetadata& indexingMetadata, Tuple tuple);
        void removeBtree(IndexingMetadata& indexingMetadata, const BPlusKey& key, SharedPagePtr node = nullptr);
        SharedPagePtr findLeaf(const IndexingMetadata& indexingMetadata, const BPlusKey& key);
        BtreeLeafIterator searchBtree(const IndexingMetadata& indexingMetadata, BPlusKey& key);
        BtreeLeafIterator beginLeaf(const IndexingMetadata &indexingMetadata);
        BtreeLeafIterator beginLeaf(const std::string& entityName, uint32_t nodeIndex);
        BtreeLeafIterator& endLeaf();
        BtreeLeafIterator& nextLeaf(BtreeLeafIterator& iter);

        void printBtree(IndexingMetadata& indexingMetadata, int nodeIndex = 0, std::string _prefix = "", bool _last = true);

    private:
        //===========================================B+Tree=Internal=========================================
        void insertBtree(IndexingMetadata& indexingMetadata, SplitNodeType result);
        SplitNodeType splitLeaf(SharedPagePtr node);
        SplitNodeType splitInternal(SharedPagePtr node);
        void removeFromLeaf(const BPlusKey& key, SharedPagePtr node);
        void removeFromInternal(const BPlusKey& key, SharedPagePtr node);

        BtreeLeafIterator endLeafBnode = BtreeLeafIterator(-1, nullptr);
    };
}



#endif //STORAGE_H
