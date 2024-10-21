//
// Created by chiendd on 27/07/2024.
//

#ifndef SLOTTED_PAGE_H
#define SLOTTED_PAGE_H
#include <vector>

#include "../entity.h"
#include "../io_manager/io_manager.h"

namespace Csql {
    class SlottedPage;
    using SharedPagePtr = std::shared_ptr<SlottedPage>;

    enum class PageType {
        free_page = 'F',
        internal_b_plus_node = 'I',
        leaf_b_plus_node = 'L',
    };

    constexpr uint32_t slottedPageHeaderDefaultSize = 1 + sizeof(uint32_t) * 4;

    class SlottedPage : public IOManager {
    public:
        SlottedPage(uint64_t _pageIndex, PageType _pageType, SharedEntityPtr _entity, uint32_t _beginFreeSpace = 0);
        SlottedPage(uint64_t _pageIndex, SharedEntityPtr _entity);
        SlottedPage() = default;
        ~SlottedPage() override = default;

        void loadType();
        void saveType();

        //Refreshes the variables based on the raw data array
        virtual void refresh();

        //Saves variables to the raw data array
        virtual void save();

        [[nodiscard]] uint32_t getFreeSpace() const;
        Tuple readTuple(uint32_t iSlot);
        void writeTuple(uint32_t iSlot);
        virtual void addTuple(const Tuple& aTuple) {};
        virtual void splitNode(SharedPagePtr left) {};
        void deleteTuple(u_int32_t iSlot);
        int lower_bound(const BPlusKey &key, int begin);

        void reassignData();

        [[nodiscard]] uint32_t get_begin_free_space() const {
            return beginFreeSpace;
        }

        void set_begin_free_space(const uint32_t begin_free_space) {
            beginFreeSpace = begin_free_space;
        }

        [[nodiscard]] uint32_t get_end_free_space() const {
            return endFreeSpace;
        }

        void set_end_free_space(const uint32_t end_free_space) {
            endFreeSpace = end_free_space;
        }

        [[nodiscard]] uint32_t get_num_slots() const {
            return numSlots;
        }

        void set_num_slots(const uint32_t num_slots) {
            numSlots = num_slots;
        }

        [[nodiscard]] std::vector<uint32_t> get_slots() const {
            return slots;
        }

        [[nodiscard]] std::vector<Tuple>& get_tuples() {
            return tuples;
        }

        [[nodiscard]] SharedEntityPtr& get_the_entity() {
            return theEntity;
        }

        void set_the_entity(const SharedEntityPtr &the_entity) {
            theEntity = the_entity;
        }

         BPlusKey get_btree_key();

        //type of page
        PageType pageType = PageType::free_page;

        //index of page
        uint32_t pageIndex;

        //index of prev node if this node is leaf
        uint32_t prevLeaf = 0;

        //index of next node if this node is leaf
        uint32_t nextLeaf = 0;

        //index of parent node
        uint32_t parentIndex = 0;

        friend class LeafBPlusNode;
        friend class InternalBPlusNode;
    protected:
        //begin of free space
        uint32_t beginFreeSpace;

        //end of free space
        uint32_t endFreeSpace;

        //number of slots in this page
        uint32_t numSlots = 0;

        std::vector<uint32_t> slots;

        std::vector<Tuple> tuples;

        SharedEntityPtr theEntity = nullptr;
    };
}



#endif //SLOTTED_PAGE_H
