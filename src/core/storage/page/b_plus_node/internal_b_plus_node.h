//
// Created by chiendd on 23/10/2024.
//

#ifndef INTERNAL_B_PLUS_NODE_H
#define INTERNAL_B_PLUS_NODE_H
#include "../slotted_page.h"

namespace Csql {
    class InternalBPlusNode : public SlottedPage {
    public:
        InternalBPlusNode(uint64_t _pageIndex, SharedEntityPtr _entity)
            : SlottedPage(_pageIndex, PageType::internal_b_plus_node, std::move(_entity), 13) {};
        InternalBPlusNode(SlottedPage &theSlottedPage);

        void refresh() override;
        void save() override;

        void addTuple(const Tuple &aTuple) override;
        void splitNode(SharedPagePtr left) override;

        void setChild(const BPlusKey &key, uint32_t left, uint32_t right);
        uint32_t indexOfChild(const BPlusKey &key);
        uint32_t getChild(const BPlusKey& key);
    };
}

#endif //INTERNAL_B_PLUS_NODE_H
