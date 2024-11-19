//
// Created by chiendd on 23/10/2024.
//

#include "free_page.h"

#include <assert.h>


FreePage::FreePage(SlottedPage &theSlottedPage) {
    this->pageIndex = theSlottedPage.pageIndex;
    this->theEntity = theSlottedPage.get_the_entity();
    memcpy(this->rawData, theSlottedPage.rawData, Configs::storageUnitSize);
}

void FreePage::refresh() {
    loadType();
    read(nextPage);
}

void FreePage::save() {
    saveType();
    write(nextPage);
}

void FreePage::addTuple(const Tuple &aTuple) {
    assert(false);
}

void FreePage::splitNode(SharedPagePtr left) {
    assert(false);
}
