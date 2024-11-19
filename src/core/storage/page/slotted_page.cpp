//
// Created by chiendd on 27/07/2024.
//

#include "slotted_page.h"

#include <cassert>

#include "../storage.h"
#include "../../util/configs.h"
#include "../../util/helpers.h"

SlottedPage::SlottedPage(uint64_t _pageIndex, PageType _pageType, SharedEntityPtr _entity, uint32_t _beginFreeSpace) {
    pageIndex = _pageIndex;
    pageType = _pageType;
    theEntity = std::move(_entity);
    beginFreeSpace = _beginFreeSpace;
    endFreeSpace = Configs::storageUnitSize;
}

SlottedPage::SlottedPage(uint64_t _pageIndex, SharedEntityPtr _entity) {
    pageIndex = _pageIndex;
    theEntity = std::move(_entity);
}

void SlottedPage::loadType() {
    bufferOffset = 0;
    char type;
    read(type);
    if (type == 'F') {
        pageType = PageType::free_page;
    } else if (type == 'I') {
        pageType = PageType::internal_b_plus_node;
    } else if (type == 'L') {
        pageType = PageType::leaf_b_plus_node;
    }
}

void SlottedPage::saveType() {
    bufferOffset = 0;
    if (pageType == PageType::free_page) {
        write('F');
    } else if (pageType == PageType::internal_b_plus_node) {
        write('I');
    } else if (pageType == PageType::leaf_b_plus_node) {
        write('L');
    }
}

void SlottedPage::refresh() {
    read(this->parentIndex);
    read(this->beginFreeSpace);
    read(this->endFreeSpace);
    read(this->numSlots);

    slots.resize(this->numSlots);
    tuples.resize(this->numSlots);

    endFreeSpace = Configs::storageUnitSize;
    for (int i = 0; i < this->numSlots; ++i) {
        read(this->slots[i]);
        endFreeSpace = this->slots[i];
    }

    beginFreeSpace = bufferOffset;


    for (int i = 0; i < this->numSlots; ++i) {
        tuples[i] = readTuple(i);
    }
}

void SlottedPage::save() {
    write(this->parentIndex);
    write(this->beginFreeSpace);
    write(this->endFreeSpace);
    write(this->numSlots);

    for (auto &slot: this->slots) {
        write(slot);
    }

    for (int i = 0; i < this->numSlots; ++i) {
        writeTuple(i);
    }
}

uint32_t SlottedPage::getSizeOfUsedBytes() const {
    return Configs::storageUnitSize - (endFreeSpace - beginFreeSpace);
}


Tuple SlottedPage::readTuple(uint32_t iSlot) {
    bufferOffset = slots.at(iSlot);
    uint32_t endOffset = (iSlot == 0 ? Configs::storageUnitSize : slots.at(iSlot - 1));

    Tuple theTuple;
    while (bufferOffset < endOffset) {
        std::string name;
        read(name);
        SqlTypes aValue;
        DataTypes type;

        if (name == SpecialKey::BTREE_KEY) {
            type = DataTypes::b_plus_key;
        } else if (name == SpecialKey::CHILD_BTREE_KEY) {
            type = DataTypes::int_type;
        } else {
            type = theEntity->getAttribute(name)->getType();
        }

        read(aValue, type);

        theTuple.insert(std::make_pair(name, aValue));
    }

    return theTuple;
}

void SlottedPage::writeTuple(uint32_t iSlot) {
    auto run = [&](const std::string &name, const DataTypes &type) {
        write(name);
        write(tuples[iSlot].at(name), type);
    };

    bufferOffset = slots.at(iSlot);
    for (auto &attribute: theEntity->getAttributes()) {
        if (tuples[iSlot].contains(attribute->getName())) {
            run(attribute->getName(), attribute->getType());
        }
    }
    if (tuples[iSlot].contains(SpecialKey::BTREE_KEY)) {
        run(SpecialKey::BTREE_KEY, DataTypes::b_plus_key);
    }
    if (tuples[iSlot].contains(SpecialKey::CHILD_BTREE_KEY)) {
        run(SpecialKey::CHILD_BTREE_KEY, DataTypes::int_type);
    }
    assert(bufferOffset - slots.at(iSlot) == Helpers::SqlTypeHandle::sizeOfTuple(tuples[iSlot]));
}

void SlottedPage::deleteTuple(u_int32_t iSlot) {
    // assert(iSlot < numSlots);
    // uint32_t sz = Helpers::SqlTypeHandle::sizeOfTuple(tuples[iSlot]);
    // shift(endFreeSpace + sz, endFreeSpace, slots[iSlot] - endFreeSpace);
    // tuples.erase(tuples.begin() + iSlot);
    // slots.erase(slots.begin() + iSlot);
    // numSlots--;
    tuples.erase(tuples.begin() + iSlot);
    reassignData();
}

int SlottedPage::lower_bound(const BPlusKey &key, int begin) {
    int l = begin;
    int r = numSlots - 1;
    if (l >= tuples.size() || key < Helpers::TupleHandle::getBTreeKey(tuples[l])) {
        return -1;
    }
    while (l < r) {
        int mid = (l + r + 1) / 2;
        if (Helpers::TupleHandle::getBTreeKey(tuples[mid]) <= key) {
            l = mid;
        } else {
            r = mid - 1;
        }
    }
    return l;
}

void SlottedPage::printLog() {
    auto print = [](std::string name, int value) {
        std::cerr << "[" + name + "=" + std::to_string(value) + "]$";
    };
    std::cerr << "#" + std::to_string(pageIndex) + ":";
    print("parentIndex", parentIndex);
    print("isLeaf", pageType == PageType::leaf_b_plus_node);


    if ((pageType == PageType::internal_b_plus_node) == tuples.size()) {
        std::cerr << "[~]";
    } else {
        std::cerr << "[";
        for (int i = (pageType == PageType::internal_b_plus_node); i < tuples.size(); ++i) {
            std::get<BPlusKey>(tuples[i].at(SpecialKey::BTREE_KEY)).log();
            std::cerr << (i + 1 == tuples.size() ? "]" : ",");
        }
    }

    if (pageType == PageType::internal_b_plus_node) {
        std::cerr << "[";
        for (int i = 0; i < tuples.size(); ++i) {
            std::cerr << std::get<SqlIntType>(tuples[i].at(SpecialKey::CHILD_BTREE_KEY)) << (
                i + 1 == tuples.size() ? "]\n" : ",");
        }
    } else {
        std::cerr << '\n';
    }

    std::cerr.flush();
}

Tuple &SlottedPage::getTuple(uint32_t i) {
    assert(i < tuples.size());
    return tuples.at(i);
}


void SlottedPage::reassignData() {
    //TODO: need optimize this function
    beginFreeSpace = slottedPageHeaderDefaultSize + (pageType == PageType::internal_b_plus_node ? 0 : sizeof(uint32_t));
    endFreeSpace = Configs::storageUnitSize;

    numSlots = tuples.size();
    slots.resize(numSlots);

    for (int i = 0; i < numSlots; ++i) {
        beginFreeSpace += sizeof(uint32_t);
        endFreeSpace -= Helpers::SqlTypeHandle::sizeOfTuple(tuples[i]);
        slots[i] = endFreeSpace;
    }
}

BPlusKey SlottedPage::get_btree_key() {
    if (this->pageType == PageType::leaf_b_plus_node) {
        assert(!tuples.empty());
        return Helpers::TupleHandle::getBTreeKey(*tuples.begin());
    } else {
        assert(tuples.size() > 1);
        return Helpers::TupleHandle::getBTreeKey(*next(tuples.begin()));
    }
}

uint32_t SlottedPage::get_child_index(uint32_t i) {
    assert(i < numSlots);
    return Helpers::TupleHandle::getBtreeChild(tuples[i]);
}
