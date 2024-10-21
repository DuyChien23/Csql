//
// Created by chiendd on 23/10/2024.
//

#ifndef FREE_PAGE_H
#define FREE_PAGE_H
#include "slotted_page.h"

namespace Csql {
    class FreePage : public SlottedPage {
    public:
        FreePage(uint64_t _pageIndex, SharedEntityPtr _entity) : SlottedPage(_pageIndex, PageType::free_page, std::move(_entity)) {};
        FreePage(SlottedPage &theSlottedPage);

        void refresh() override;
        void save() override;

        uint32_t getNextFreePage() {
            return nextPage;
        }

        void setNextFreePage(uint32_t _nextPage) {
            nextPage = _nextPage;
        }

        void addTuple(const Tuple &aTuple) override;
        void splitNode(SharedPagePtr left) override;
    protected:
        uint32_t nextPage = 0;
    };
}

#endif //FREE_PAGE_H
