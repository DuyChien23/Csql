//
// Created by chiendd on 27/07/2024.
//

#ifndef SLOTTED_PAGE_H
#define SLOTTED_PAGE_H
#include <vector>

#include "../entity.h"
#include "../io_manager/io_manager.h"

namespace Csql {
    class SlottedPage : public IOManager {
    protected:
        //index of block in entity file
        uint32_t pageIndex;

        //next page depend this page is free_page or data_page
        uint32_t nextPage = 0;

        //begin of free space
        uint32_t beginFreeSpace;

        //end of free space
        uint32_t endFreeSpace = Configs::storageUnitSize;

        //number of slots in this page
        uint32_t numSlots = 0;

        std::vector<uint32_t> slots;

        std::vector<Tuple> tuples;

        SharedEntityPtr theEntity = nullptr;

    public:
        SlottedPage(uint64_t pageIndex, SharedEntityPtr entity) : pageIndex(pageIndex), theEntity(entity), beginFreeSpace(getHeaderSize()) {};

        ~SlottedPage() = default;

        //Refreshes the variables based on the raw data array
        void refresh();

        //Saves variables to the raw data array
        void save();

        uint32_t getHeaderSize();
        uint32_t getFreeSpace();
        Tuple* readTuple(uint32_t iSlot);
        void writeTuple(uint32_t iSlot);
        bool addTuple(const Tuple& aTuple);
        void deleteTuple(u_int32_t iSlot);


        [[nodiscard]] uint32_t get_page_index() const {
            return pageIndex;
        }

        void set_page_index(const uint32_t page_index) {
            pageIndex = page_index;
        }

        [[nodiscard]] uint32_t get_next_page() const {
            return nextPage;
        }

        void set_next_page(const uint32_t next_page) {
            nextPage = next_page;
        }

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

        void set_slots(std::vector<uint32_t> const slots) {
            this->slots = slots;
        }

        [[nodiscard]] std::vector<Tuple>& get_tuples() {
            return tuples;
        }

        void set_tuples(std::vector<Tuple> const tuples) {
            this->tuples = tuples;
        }

        [[nodiscard]] SharedEntityPtr& get_the_entity() {
            return theEntity;
        }

        void set_the_entity(const SharedEntityPtr &the_entity) {
            theEntity = the_entity;
        }
    };

    using SharedPagePtr = std::shared_ptr<SlottedPage>;
}



#endif //SLOTTED_PAGE_H
