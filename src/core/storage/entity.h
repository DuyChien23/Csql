//
// Created by chiendd on 21/07/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <functional>

#include "attribute.h"
#include "tuple.h"
#include "io_manager/io_manager.h"

namespace Csql {
    using AttributeVisitor = std::function<void(Attribute*)>;
        class Entity : public IOManager {
        public:
            Entity(std::string dbName, std::string entityName) {
                this->dbName = dbName;
                this->name = entityName;
            };
            Entity(std::string entityName) {
                this->name = entityName;
            };
            ~Entity() = default;

            void save();
            void refresh();

            Reference* readReference();
            void writeReference(Reference *aReference);

            Attribute* readAttribute();
            void writeAttribute(Attribute *anAttribute);

            Tuple baseNullTuple();
            void eachAttribute(const AttributeVisitor &attributeVisitor);

            void addAttribute(Attribute *attribute);
            Attribute* getAttribute(const std::string& name);
            AttributeList* getAttributes();
            void setDatabaseName(std::string dbName);
            void setFirstDataPage(uint64_t firstDataBlock);
            uint64_t getFirstDataPage();
            void setFirstFreePage(uint64_t firstFreeBlock);
            bool hasDataPage();
            bool hasFreePage();
            uint64_t getFirstFreePage();
            uint64_t getIndexNewPage();
            std::string getName();
        protected:
            AttributeList attributes;

            std::string dbName;

            std::string name;

            uint64_t firstDataPage = 0;

            uint64_t firstFreePage = 0;

            uint64_t nPage = 1;
        };

    using SharedEntityPtr = std::shared_ptr<Entity>;
}

#endif //ENTITY_H
