//
// Created by chiendd on 23/10/2024.
//

#include "leaf_b_plus_node.h"

#include <assert.h>
#include <iostream>

#include "../free_page.h"
#include "../../../util/helpers.h"
#include "../../expression/other_expression.h"


LeafBPlusNode::LeafBPlusNode(SlottedPage &theSlottedPage) {
    this->pageIndex = theSlottedPage.pageIndex;
    this->theEntity = theSlottedPage.get_the_entity();

    if (theSlottedPage.pageType == PageType::free_page) {
        this->pageType = PageType::leaf_b_plus_node;
        this->prevLeaf = 0;
        this->nextLeaf = 0;
        beginFreeSpace = slottedPageHeaderDefaultSize + 8;
        endFreeSpace = Configs::storageUnitSize;
        numSlots = 0;
    } else {
        memcpy(this->rawData, theSlottedPage.rawData, Configs::storageUnitSize);
        refresh();
    }
}

void LeafBPlusNode::refresh() {
    loadType();
    read(prevLeaf);
    read(nextLeaf);
    SlottedPage::refresh();
}

void LeafBPlusNode::save() {
    saveType();
    write(prevLeaf);
    write(nextLeaf);
    SlottedPage::save();
}

void LeafBPlusNode::addTuple(const Tuple &aTuple) {
    const BPlusKey &key = Helpers::TupleHandle::getBTreeKey(aTuple);
    int index = lower_bound(key, 0);
    assert(index == -1 || Helpers::TupleHandle::getBTreeKey(tuples[index]) != key);
    tuples.insert(tuples.begin() + index + 1, aTuple);
    reassignData();
}

void LeafBPlusNode::splitNode(SharedPagePtr left) {
    int mid = numSlots / 2;

    left->tuples.assign(this->tuples.begin(), this->tuples.begin() + mid);
    left->reassignData();

    this->tuples.erase(this->tuples.begin(), this->tuples.begin() + mid);
    reassignData();
}

void LeafBPlusNode::borrowKeyFromRightLeaf(SharedPagePtr next, SharedPagePtr parent) {
    uint32_t sz = Helpers::SqlTypeHandle::sizeOfTuple(next->get_tuples().front());
    tuples.push_back(next->get_tuples().front());
    slots.push_back(slots.back() - sz);
    numSlots++;

    next->deleteTuple(0);

    for (int i = 0; i < parent->numSlots; ++i) {
        if (parent->get_child_index(i) == next->pageIndex) {
            parent->tuples[i].at(SpecialKey::BTREE_KEY) = Helpers::TupleHandle::getBTreeKey(next->get_tuples().front());
            break;
        }
    }
}

void LeafBPlusNode::borrowKeyFromLeftLeaf(SharedPagePtr prev, SharedPagePtr parent) {
    tuples.insert(tuples.begin(), prev->get_tuples().back());
    reassignData();

    prev->deleteTuple(prev->get_num_slots() - 1);

    for (int i = 0; i < parent->numSlots; ++i) {
        if (parent->get_child_index(i) == this->pageIndex) {
            parent->tuples[i].at(SpecialKey::BTREE_KEY) = Helpers::TupleHandle::getBTreeKey(this->get_tuples().front());
            break;
        }
    }
}

void LeafBPlusNode::mergeLeafNode(SharedPagePtr next, SharedPagePtr parent) {
    tuples.insert(tuples.end(), next->get_tuples().begin(), next->get_tuples().end());
    reassignData();

    nextLeaf = next->nextLeaf;

    for (int i = 0; i < parent->tuples.size(); ++i) {
        if (parent->get_child_index(i) == next->pageIndex) {
            parent->deleteTuple(i);
            return;
        }
    }
    //
    // std::cerr << "Parent:   "; parent->printLog();
    // std::cerr << "Current:  "; this->printLog();
    // std::cerr << "Next:     "; next->printLog();
    assert(false);
}
