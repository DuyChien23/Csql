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
        return this->get_child_index(indexOfChild(key));
    }

    void InternalBPlusNode::borrowKeyFromRightInternal(SharedPagePtr next, SharedPagePtr parent, int index) {
        Tuple theTuple;
        theTuple.insert(std::make_pair(SpecialKey::BTREE_KEY, parent->getTuple(index + 1).at(SpecialKey::BTREE_KEY)));
        parent->getTuple(index + 1).at(SpecialKey::BTREE_KEY) = next->get_btree_key();

        theTuple.insert(std::make_pair(SpecialKey::CHILD_BTREE_KEY, SqlIntType(next->get_child_index(0))));

        tuples.push_back(theTuple);
        reassignData();
        next->deleteTuple(0);
    }

    void InternalBPlusNode::borrowKeyFromLeftInternal(SharedPagePtr prev, SharedPagePtr parent, int index) {
        this->tuples[0].at(SpecialKey::BTREE_KEY) = parent->getTuple(index).at(SpecialKey::BTREE_KEY);
        parent->getTuple(index).at(SpecialKey::BTREE_KEY) = prev->get_tuples().back().at(SpecialKey::BTREE_KEY);

        Tuple theTuple;
        theTuple.insert(std::make_pair(SpecialKey::BTREE_KEY, BPlusKey()));
        theTuple.insert(std::make_pair(SpecialKey::CHILD_BTREE_KEY, prev->get_tuples().back().at(SpecialKey::CHILD_BTREE_KEY)));

        this->tuples.insert(tuples.begin(), theTuple);

        reassignData();
        prev->deleteTuple(prev->tuples.size() - 1);
    }

    void InternalBPlusNode::mergeInternalNode(SharedPagePtr next, SharedPagePtr parent, int index) {
        int modifyIndex = tuples.size();
        tuples.insert(tuples.end(), next->get_tuples().begin(), next->get_tuples().end());
        tuples[modifyIndex].at(SpecialKey::BTREE_KEY) = parent->getTuple(index + 1).at(SpecialKey::BTREE_KEY);
        parent->deleteTuple(index + 1);

        reassignData();
    }
}
