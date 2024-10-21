//
// Created by chiendd on 23/10/2024.
//

#ifndef LEAF_B_PLUS_NODE_H
#define LEAF_B_PLUS_NODE_H
#include "../slotted_page.h"

namespace Csql {
    class LeafBPlusNode : public SlottedPage {
    public:
        LeafBPlusNode(uint64_t _pageIndex, SharedEntityPtr _entity) : SlottedPage(_pageIndex, PageType::leaf_b_plus_node, std::move(_entity), 17) {};
        LeafBPlusNode(SlottedPage &theSlottedPage);

        void refresh() override;
        void save() override;

        void addTuple(const Tuple &aTuple) override;
        void splitNode(SharedPagePtr left);
    };
}

#endif //LEAF_B_PLUS_NODE_H
