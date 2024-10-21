//
// Created by chiendd on 23/10/2024.
//

#include "internal_b_plus_node.h"

#include "../../../util/helpers.h"

namespace Csql {
    InternalBPlusNode::InternalBPlusNode(SlottedPage &theSlottedPage) {
        this->pageIndex = theSlottedPage.pageIndex;
        this->theEntity = theSlottedPage.get_the_entity();

        if (theSlottedPage.pageType == PageType::free_page) {
            this->pageType = PageType::internal_b_plus_node;
            beginFreeSpace = 13;
            endFreeSpace = Configs::storageUnitSize;
            numSlots = 0;
        } else {
            memcpy(this->rawData, theSlottedPage.rawData, Configs::storageUnitSize);
            refresh();
        }
    }

    void InternalBPlusNode::refresh() {
        loadType();
        SlottedPage::refresh();
    }

    void InternalBPlusNode::save() {
        loadType();
        SlottedPage::save();
    }

    void InternalBPlusNode::addTuple(const Tuple &aTuple) {
        if (tuples.size() <= 1) {
            tuples.push_back(aTuple);
        } else {
            const BPlusKey &key = Helpers::TupleHandle::getBTreeKey(aTuple);
            int index = indexOfChild(key);
            tuples.insert(tuples.begin() + index + 1, aTuple);
        }
        reassignData();
    }

    void InternalBPlusNode::splitNode(SharedPagePtr left) {
        int mid = (numSlots - 1) / 2;

        left->tuples.assign(this->tuples.begin(), this->tuples.begin() + mid + 1);
        left->reassignData();

        this->tuples.erase(this->tuples.begin(), this->tuples.begin() + mid + 1);
        reassignData();
    }

    void InternalBPlusNode::setChild(const BPlusKey &key, uint32_t left, uint32_t right) {
        int i = indexOfChild(key);
        tuples.insert(tuples.begin() + i + 1, Helpers::TupleHandle::baseInternalBNode(key, right));
        tuples[i].at(SpecialKey::CHILD_BTREE_KEY) = SqlIntType(left);
        reassignData();
    }

    uint32_t InternalBPlusNode::indexOfChild(const BPlusKey &key) {
        int index = lower_bound(key, 1);
        if (index == -1) return 0;
        return index;
    }

    uint32_t InternalBPlusNode::getChild(const BPlusKey &key) {
        return Helpers::SqlTypeHandle::concretizeSqlType<SqlIntType>(tuples[indexOfChild(key)].at(SpecialKey::CHILD_BTREE_KEY));
    }
}
