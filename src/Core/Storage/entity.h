//
// Created by chiendd on 21/07/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "attribute.h"
#include "IO_Manager/io_manager.h"

namespace Csql {
    class Entity : public IOManager {
    public:
        Entity(std::string dbName, std::string entityName) {
            this -> dbName = dbName;
            this -> name = entityName;
        };

        ~Entity() = default;

        void save();

        void refresh();

        void addAttribute(Attribute *attribute);

        Attribute* getAttribute(std::string name);

        AttributeList* getAttributes();

        void setFirstDataBlock(uint64_t firstDataBlock) {
            this -> firstDataBlock = firstDataBlock;
        };

        uint64_t getFirstDataBlock() { return firstDataBlock; }

        void setFirstFreeBlock(uint64_t firstFreeBlock) {
            this -> firstFreeBlock = firstFreeBlock;
        };

        uint64_t getFirstFreeBlock() { return firstFreeBlock; }

        Attribute* readAttribute();

        void writeAttribute(Attribute *anAttribute);

    protected:
        AttributeList attributes;

        std::string dbName;

        std::string name;

        uint64_t firstDataBlock;

        uint64_t firstFreeBlock;
    };

    using SharedEntityPtr = std::shared_ptr<Entity>;
}

#endif //ENTITY_H
