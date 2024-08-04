//
// Created by chiendd on 27/07/2024.
//

#ifndef DATA_PAGE_H
#define DATA_PAGE_H
#include "slotted_page.h"
#include "../entity.h"

namespace Csql {
    class DataPage : public SlottedPage {
    public:
        DataPage(uint32_t pageIndex, SharedEntityPtr entity) : SlottedPage(pageIndex, entity) {};

        ~DataPage() = default;
    };
}


#endif //DATA_PAGE_H
