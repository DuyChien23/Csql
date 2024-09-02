//
// Created by chiendd on 12/08/2024.
//

#include "storage.h"

#include <unistd.h>

namespace Csql {
    void Storage::writePage(SharedPagePtr aPage) {
        aPage->save();
        SharedPagePtr needWritingPage = aPage;
        if (Configs::cacheMaxSize) {
            std::string theKey = createKey(aPage->get_the_entity()->getName(), aPage->get_page_index());
            needWritingPage = pageCache->put(theKey, aPage);
        }
        writePageUncache(needWritingPage);
    }

    void Storage::writePageUncache(SharedPagePtr aPage) {
        std::fstream &f = getFstream(aPage->get_the_entity()->getName());
        f.seekg(aPage->get_page_index() * Configs::storageUnitSize);
        aPage->encode(f);
    }

    void Storage::readPage(const std::string &entityName, uint32_t pageIndex, SharedPagePtr aPage) {
        std::string theKey = createKey(entityName, pageIndex);
        if (pageCache->contains(theKey)) {
            aPage = pageCache->get(theKey);
        } else {
            readPageUncache(entityName, pageIndex, aPage);
            SharedPagePtr needWritingPage = pageCache->put(theKey, aPage);
            needWritingPage->set_page_index(pageIndex);
            writePageUncache(needWritingPage);
        }

        aPage->refresh();
    }

    void Storage::readPageUncache(const std::string &entityName, uint32_t pageIndex, SharedPagePtr aPage) {
        std::fstream &f = getFstream(entityName);
        f.seekg(pageIndex * Configs::storageUnitSize);
        aPage = std::make_shared<SlottedPage>(pageIndex, getEntity(entityName));
        aPage->decode(f);
    }

    void Storage::deleteEntityFile(std::string entityName) {
        std::filesystem::remove(createEntityPath(entityName));
        mEntity.erase(entityName);
        mFstream.erase(entityName);

        if (listEntityNames) {
            auto it = std::find(listEntityNames->begin(), listEntityNames->end(), entityName);
            if (it != listEntityNames->end()) {
                listEntityNames->erase(it);
            }
        }
    }

    void Storage::eachEntity(const EntityVisitor& entityVisitor) {
        for (auto &entityName : *getListEntityNames()) {
            entityVisitor(getEntity(entityName));
        }
    }

    SharedPagePtr Storage::popFreePage(const std::string &entityName) {
        SharedEntityPtr theEntity = getEntity(entityName);

        if (theEntity->getFirstFreePage() == 0) {
            return std::make_shared<SlottedPage>(theEntity->getIndexNewPage(), theEntity);
        } else {
            SharedPagePtr thePage;
            readPage(entityName, theEntity->getFirstFreePage(), thePage);
            theEntity->setFirstFreePage(thePage->get_next_page());
            return thePage;
        }
    }

    void Storage::pushDataPage(const std::string &entityName, SharedPagePtr aPage) {
        SharedEntityPtr theEntity = getEntity(entityName);
        aPage->set_next_page(theEntity->getFirstDataPage());
        theEntity->setFirstDataPage(aPage->get_page_index());
        writePage(aPage);
    }

    void Storage::eachDataPage(const std::string &entityName, const PageVisitor &pageVisitor) {
        SharedEntityPtr theEntity = getEntity(entityName);
        SharedPagePtr thePage;

        for (uint32_t index = theEntity->getFirstDataPage(); index != 0; index = thePage->get_next_page()) {
            readPage(entityName, index, thePage);
            pageVisitor(thePage);
        }
    }
}
