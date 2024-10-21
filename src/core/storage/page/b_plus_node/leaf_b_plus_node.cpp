//
// Created by chiendd on 23/10/2024.
//

#include "leaf_b_plus_node.h"

#include <assert.h>

#include "../free_page.h"
#include "../../../util/helpers.h"
#include "../../expression/other_expression.h"

namespace Csql {
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
}
