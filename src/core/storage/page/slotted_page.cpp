//
// Created by chiendd on 27/07/2024.
//

#include "slotted_page.h"

#include <assert.h>

#include "../../util/configs.h"
#include "../../util/helpers.h"

namespace Csql {
    void SlottedPage::refresh() {
        bufferOffset = 0;
        read(this->nextPage);
        read(this->beginFreeSpace);
        read(this->endFreeSpace);
        read(this->numSlots);

        slots.resize(this->numSlots);
        tuples.resize(this->numSlots);

        for (int i = 0; i < this->numSlots; ++i) {
            read(this->slots[i]);
        }

        for (int i = 0; i < this->numSlots; ++i) {
            tuples[i] = *readTuple(i);
        }
    }

    void SlottedPage::save() {
        bufferOffset = 0;
        write(this->nextPage);
        write(this->beginFreeSpace);
        write(this->endFreeSpace);
        write(this->numSlots);

        for (auto &slot : this->slots) {
            write(slot);
        }

        for (int i = 0; i < this->numSlots; ++i) {
            writeTuple(i);
        }
    }

    uint32_t SlottedPage::getHeaderSize() {
        return sizeof(this->nextPage)
            + sizeof(this->beginFreeSpace)
            + sizeof(this->endFreeSpace)
            + sizeof(this->numSlots);
    }

    uint32_t SlottedPage::getFreeSpace() {
        return (numSlots ? endFreeSpace - beginFreeSpace : Configs::storageUnitSize - getHeaderSize());
    }


    Tuple *SlottedPage::readTuple(uint32_t iSlot) {
        bufferOffset = slots.at(iSlot);
        uint32_t endOffset = (iSlot == 0 ? Configs::storageUnitSize : slots.at(iSlot - 1));

        Tuple *theTuple = new std::map<std::string, SqlTypes>;
        for (; bufferOffset < endOffset;) {
            std::string name;
            read(name);
            SqlTypes aValue;
            read(aValue, theEntity->getAttribute(name)->getType());
            theTuple->insert(std::make_pair(name, aValue));
        }

        return theTuple;
    }

    void SlottedPage::writeTuple(uint32_t iSlot) {
        bufferOffset = slots.at(iSlot);
        for (auto &attribute : *theEntity->getAttributes()) {
            if (tuples[iSlot].contains(attribute->getName())) {
                write(attribute->getName());
                write(tuples[iSlot].at(attribute->getName()), attribute->getType());
            }
        }
    }

    bool SlottedPage::addTuple(const Tuple &aTuple) {
        uint32_t sz = Helpers::SqlTypeHandle::sizeOfTuple(aTuple);
        if (this->getFreeSpace() < sz + sizeof(uint32_t)) return false;
        beginFreeSpace += sizeof(uint32_t);
        endFreeSpace -= sz;
        numSlots++;
        slots.push_back(endFreeSpace);
        tuples.push_back(aTuple);
        return true;
    }

}
