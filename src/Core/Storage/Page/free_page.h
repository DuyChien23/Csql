//
// Created by chiendd on 29/07/2024.
//

#ifndef FREE_PAGE_H
#define FREE_PAGE_H
#include "slotted_page.h"
#include "../entity.h"

namespace Csql {
    class FreePage : public SlottedPage {
    public:
        FreePage(uint32_t pageIndex, SharedEntityPtr entity) : SlottedPage(pageIndex, entity) {};

        ~FreePage() = default;
    };
}



#endif //FREE_PAGE_H
