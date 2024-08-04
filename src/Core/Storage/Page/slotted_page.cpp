//
// Created by chiendd on 27/07/2024.
//

#include "slotted_page.h"

namespace Csql {
    void SlottedPage::refresh() {
        bufferOffset = 0;
        read(this -> nextPage);
        read(this -> beginFreeSpace);
        read(this -> endFreeSpace);
        read(this -> numSlots);

        slots.resize(this -> numSlots);
        tuples.resize(this -> numSlots);

        for (int i = 0; i < this -> numSlots; ++i) {
            read(this -> slots[i]);
        }
    }

    void SlottedPage::save() {
        bufferOffset = 0;
        write(this -> nextPage);
        write(this -> beginFreeSpace);
        write(this -> endFreeSpace);
        write(this -> numSlots);

        for (auto &slot : this -> slots) {
            write(slot);
        }

        for (auto tuple : this -> tuples) {
            write(tuple);
        }
    }

    uint32_t SlottedPage::getHeaderSize() {
        return sizeof(this -> nextPage)
            + sizeof(this -> beginFreeSpace)
            + sizeof(this -> endFreeSpace)
            + sizeof(this -> numSlots);
    }

    uint32_t SlottedPage::getTupleSize(const Tuple& aTuple) {
        uint32_t countSize = 0;
        for (auto &element : aTuple) {
            if (!theEntity->getAttribute(element.first)) {
                continue;
            }

            countSize += sizeof (uint32_t);
            countSize += sizeof element.first;
            countSize += sizeof element.second;

            if (theEntity->getAttribute(element.first)->getType() == DataTypes::varchar_type) {
                countSize += sizeof (uint32_t);
            }
        }
        return countSize;
    }

    uint32_t SlottedPage::getFreeSpace() {
        return endFreeSpace - beginFreeSpace;
    }


    Tuple *SlottedPage::readTuple(uint32_t iSlot) {
        bufferOffset = slots.at(iSlot);
        uint32_t endOffset = (iSlot + 1 == numSlots ? Config::storageUnitSize : slots.at(iSlot + 1));

        Tuple theTuple;
        for (; bufferOffset < endOffset;) {
            std::string name;
            read(name);
            SqlTypes aValue;
            read(aValue, theEntity->getAttribute(name)->getType());
            theTuple[name] = aValue;
        }

        return &theTuple;
    }

    void SlottedPage::writeTuple(const Tuple &aTuple) {
        for (auto &attribute : *theEntity->getAttributes()) {
            if (aTuple.contains(attribute->getName())) {
                write(attribute->getName());
                write(aTuple.at(attribute->getName()));
            }
        }
    }
}
