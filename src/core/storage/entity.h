//
// Created by chiendd on 21/07/2024.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <functional>

#include "attribute.h"
#include "io_manager/io_manager.h"

namespace Csql {
    using AttributeVisitor = std::function<void(Attribute*)>;
        class Entity : public IOManager {
        public:
            Entity(std::string dbName, std::string entityName) {
                this->dbName = dbName;
                this->name = entityName;
            };

            ~Entity() = default;

            void save();

            void refresh();

            void addAttribute(Attribute *attribute);

            Attribute* getAttribute(const std::string& name);

            AttributeList* getAttributes();

            void setFirstDataPage(uint64_t firstDataBlock) {
                this->firstDataPage = firstDataBlock;
            };

            uint64_t getFirstDataPage() { return firstDataPage; }

            void setFirstFreePage(uint64_t firstFreeBlock) {
                this->firstFreePage = firstFreeBlock;
            };

            bool hasDataPage() {
                return firstDataPage;
            };

            bool hasFreePage() {
                return firstFreePage != 0;
            }

            uint64_t getFirstFreePage() { return firstFreePage; }

            uint64_t getIndexNewPage() { return nPage++; };

            std::string getName() { return name; }

            Reference* readReference();
            void writeReference(Reference *aReference);

            Attribute* readAttribute();
            void writeAttribute(Attribute *anAttribute);

            Tuple baseNullTuple();
            void eachAttribute(const AttributeVisitor &attributeVisitor);

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
