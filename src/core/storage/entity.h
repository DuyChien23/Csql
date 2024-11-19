//
// Created by chiendd on 21/07/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <functional>

#include "attribute.h"
#include "io_manager/io_manager.h"

struct IndexingMetadata;

using AttributeVisitor = std::function<void(Attribute *)>;
using IndexingVisitor = std::function<void(IndexingMetadata &, bool)>;

struct IndexingMetadata {
    std::string entityName;
    uint32_t root = 0;
    std::vector<std::string> keys;
    bool isClustered;

    IndexingMetadata() = default;

    IndexingMetadata(std::string _entityName, std::vector<std::string> _keys, bool _isClustered) {
        entityName = std::move(_entityName);
        keys = std::move(_keys);
        isClustered = _isClustered;
    }

    uint32_t getMaximalNumKey() {
        return 10;
    }

    bool operator==(const IndexingMetadata &op) const {
        if (keys.size() != op.keys.size()) return false;
        for (int i = 0; i < keys.size(); ++i) {
            if (keys[i] != op.keys[i]) return false;
        }
        return true;
    }
};

class Entity : public IOManager {
public:
    Entity(std::string dbName, std::string entityName) {
        this->dbName = std::move(dbName);
        this->name = std::move(entityName);
    };

    explicit Entity(std::string entityName) {
        this->name = std::move(entityName);
    };

    ~Entity() override = default;

    void save();

    void refresh();

    Reference *readReference();

    void writeReference(Reference *aReference);

    Attribute *readAttribute();

    void writeAttribute(Attribute *anAttribute);

    Tuple baseNullTuple();

    void eachAttribute(const AttributeVisitor &attributeVisitor);

    void eachIndexing(const IndexingVisitor &indexingVisitor);

    void addAttribute(Attribute *attribute);

    Attribute *getAttribute(const std::string &name);

    void addIndexingMetadata(std::vector<std::string> keys, bool isClustered);

    void addIndexingMetadata(std::string key, bool isClustered);

    AttributeList &getAttributes();

    void setDatabaseName(std::string dbName);

    void setFirstFreePage(uint64_t firstFreeBlock);

    [[nodiscard]] bool hasFreePage() const;

    [[nodiscard]] uint64_t getFirstFreePage() const;

    uint64_t getIndexNewPage();

    std::string getName();

    void setClusteredKey(const std::string &name);

    std::string getClusteredKey();

protected:
    AttributeList attributes;
    std::vector<IndexingMetadata> indexingMetadata;
    std::string dbName;
    std::string name;
    std::string clusteredKey;
    uint64_t firstFreePage = 0;
    uint64_t nPage = 1;
};

using SharedEntityPtr = std::shared_ptr<Entity>;


#endif //ENTITY_H
