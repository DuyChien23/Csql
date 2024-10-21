//
// Created by chiendd on 12/08/2024.
//

#include "storage.h"

#include <unistd.h>

#include "../util/errors.h"
#include "../util/helpers.h"
#include "page/free_page.h"
#include "page/b_plus_node/internal_b_plus_node.h"
#include "page/b_plus_node/leaf_b_plus_node.h"

#define GET_BTREE_KEY(aTuple) Helpers::TupleHandle::getBTreeKey(aTuple)

#define INTERNAL_CAST(node) dynamic_cast<InternalBPlusNode *>(node.get())
#define LEAF_CAST(node) dynamic_cast<LeafBPlusNode *>(node.get())

namespace Csql {
    void Storage::writePage(SharedPagePtr& aPage) {
        aPage->save();
        SharedPagePtr needWritingPage = aPage;
        if (Configs::cacheMaxSize) {
            std::string theKey = createKey(aPage->get_the_entity()->getName(), aPage->pageIndex);
            needWritingPage = pageCache->put(theKey, aPage);
        }
        writePageUncache(needWritingPage);
    }

    void Storage::writePageUncache(SharedPagePtr& aPage) {
        std::fstream &f = getFstream(aPage->get_the_entity()->getName());
        f.seekg(aPage->pageIndex * Configs::storageUnitSize);
        aPage->encode(f);
    }

    void Storage::readPage(const std::string &entityName, uint32_t pageIndex, SharedPagePtr& aPage) {
        std::string theKey = createKey(entityName, pageIndex);
        if (pageCache->contains(theKey)) {
            aPage = pageCache->get(theKey);
        } else {
            readPageUncache(entityName, pageIndex, aPage);
            SharedPagePtr needWritingPage = pageCache->put(theKey, aPage);
            needWritingPage->pageIndex = pageIndex;
            writePageUncache(needWritingPage);
        }

        aPage->refresh();
    }

    void Storage::readPageUncache(const std::string &entityName, uint32_t pageIndex, SharedPagePtr& aPage) {
        std::fstream &f = getFstream(entityName);
        f.seekg(pageIndex * Configs::storageUnitSize);
        auto temp = std::make_shared<SlottedPage>(pageIndex, getEntity(entityName));
        temp->decode(f);
        temp->loadType();
        if (temp->pageType== PageType::leaf_b_plus_node) {
            aPage = std::make_shared<LeafBPlusNode>(*temp);
        } else if (aPage->pageType == PageType::internal_b_plus_node) {
            aPage = std::make_shared<InternalBPlusNode>(*temp);
        } else {
            aPage = std::make_shared<FreePage>(*temp);
        }
    }

    void Storage::deleteEntityFile(const std::string& entityName) {
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
            if (thePage->pageType != PageType::free_page) {
                throw Errors("popFreePage: the page is not free page");
            }
            auto *tempFreePage = dynamic_cast<FreePage *>(thePage.get());
            theEntity->setFirstFreePage(tempFreePage->getNextFreePage());
            return thePage;
        }
    }

    //===========================================B+Tree=Interface=========================================
    void Storage::setBTree(IndexingMetadata& indexingMetadata, Tuple tuple) {
        SharedPagePtr leaf;
        if (indexingMetadata.root == 0) {
            leaf = std::make_shared<LeafBPlusNode>(*popFreePage(indexingMetadata.entityName));
            indexingMetadata.root = leaf->pageIndex;
        } else {
            leaf = findLeaf(indexingMetadata, GET_BTREE_KEY(tuple));
        }

        leaf->addTuple(tuple);
        if (leaf->getFreeSpace() < Configs::storageFreeSpaceSize) {
            insertBtree(indexingMetadata, splitLeaf(leaf));
        } else {
            writePage(leaf);
        }
    }

    SharedPagePtr Storage::findLeaf(const IndexingMetadata &indexingMetadata, const BPlusKey &key) {
        SharedPagePtr node;
        uint32_t nodeIndex = indexingMetadata.root;

        while (true) {
            readPage(indexingMetadata.entityName, nodeIndex, node);
            if (node->pageType == PageType::leaf_b_plus_node) return node;
            nodeIndex = INTERNAL_CAST(node)->getChild(key);
        }
    }

    BtreeLeafIterator Storage::searchBtree(const IndexingMetadata &indexingMetadata, BPlusKey& key) {
        SharedPagePtr node = findLeaf(indexingMetadata, key);
        return BtreeLeafIterator(node->lower_bound(key, 0), node);
    }

    BtreeLeafIterator Storage::beginLeaf(const IndexingMetadata &indexingMetadata) {
        if (!indexingMetadata.root) return endLeaf();

        uint32_t nodeIndex = indexingMetadata.root;
        while(true) {
            SharedPagePtr node;
            readPage(indexingMetadata.entityName, nodeIndex, node);

            if (node->pageType == PageType::leaf_b_plus_node) {
                return BtreeLeafIterator(0, node);
            }

            nodeIndex = Helpers::SqlTypeHandle::concretizeSqlType<SqlIntType>(node->get_tuples().begin()->at(SpecialKey::CHILD_BTREE_KEY));
        }
    }

    BtreeLeafIterator& Storage::endLeaf() {
        return endLeafBnode;
    }

    BtreeLeafIterator& Storage::nextLeaf(BtreeLeafIterator &iter) {
        iter.index++;
        if (iter.index >= iter.page->get_num_slots()) {
            iter.index = 0;
            int nextIndex = dynamic_cast<LeafBPlusNode*>(iter.page.get())->nextLeaf;
            if (nextIndex == 0) {
                return endLeaf();
            }
            readPage(iter.page->get_the_entity()->getName(), nextIndex, iter.page);
        }
        return iter;
    }

    void Storage::printBtree(IndexingMetadata &indexingMetadata, int nodeIndex, std::string _prefix, bool _last)  {
        if (nodeIndex == 0) nodeIndex = indexingMetadata.root;
        std::cerr << _prefix << "├ [";

        SharedPagePtr node;
        readPage(indexingMetadata.entityName, nodeIndex, node);

        for (int i = (node->pageType == PageType::leaf_b_plus_node ? 0 : 1); i < node->get_num_slots(); ++i) {
            Helpers::TupleHandle::getBTreeKey(node->get_tuples().at(i)).log();
            if (i != node->get_num_slots() - 1) {
                std::cerr << ", ";
            }
        }

        std::cerr << "]";

        if (node->pageType == PageType::leaf_b_plus_node) {
            std::cerr << node->prevLeaf << "-+-" << node->pageIndex << "-+-" << node->nextLeaf;
        }
        std::cerr << std::endl;

        _prefix += _last ? "   " : "╎  ";

        if (node->pageType == PageType::internal_b_plus_node) {
            for (int i = 0; i < node->get_num_slots(); ++i) {
                bool _last = (i == node->get_num_slots() - 1);
                printBtree(indexingMetadata, Helpers::TupleHandle::getBtreeChild(node->get_tuples().at(i)), _prefix, _last);
            }
        }
    }

    //===========================================B+Tree=Internal=========================================
    void Storage::insertBtree(IndexingMetadata& indexingMetadata, SplitNodeType result) {
        auto [key, left, right] = result;

        if (!left->parentIndex) {
            SharedPagePtr root = std::make_shared<InternalBPlusNode>(*popFreePage(indexingMetadata.entityName));
            left->parentIndex = right->parentIndex = root->pageIndex;
            root->addTuple(Helpers::TupleHandle::baseInternalBNode(key, left->pageIndex));
            root->addTuple(Helpers::TupleHandle::baseInternalBNode(key, right->pageIndex));
            writePage(left);
            writePage(right);
            writePage(root);
            indexingMetadata.root = root->pageIndex;
            return;
        }

        SharedPagePtr parent;
        readPage(left->get_the_entity()->getName(), left->parentIndex, parent);
        INTERNAL_CAST(parent)->setChild(key, left->pageIndex, right->pageIndex);

        if (parent->getFreeSpace() < Configs::storageFreeSpaceSize) {
            insertBtree(indexingMetadata, splitInternal(parent));
        } else {
            writePage(parent);
        }
    }

    SplitNodeType Storage::splitLeaf(SharedPagePtr node) {
        SharedPagePtr left = std::make_shared<LeafBPlusNode>(*popFreePage(node->get_the_entity()->getName()));
        LEAF_CAST(node)->splitNode(left);
        left->parentIndex = node->parentIndex;
        left->nextLeaf = node->pageIndex;
        left->prevLeaf = node->prevLeaf;
        node->prevLeaf = left->pageIndex;
        writePage(left);
        writePage(node);
        if (left->prevLeaf) {
            SharedPagePtr prev;
            readPage(node->get_the_entity()->getName(), left->prevLeaf, prev);
            prev->nextLeaf = left->pageIndex;
            writePage(prev);
        }
        return std::make_tuple(node->get_btree_key(), left, node);
    }

    SplitNodeType Storage::splitInternal(SharedPagePtr node) {
        SharedPagePtr left = std::make_shared<InternalBPlusNode>(*popFreePage(node->get_the_entity()->getName()));
        node->splitNode(left);
        left->parentIndex = node->parentIndex;
        auto key = GET_BTREE_KEY(*node->get_tuples().begin());

        for (auto& child : left->get_tuples()) {
            auto childIndex = Helpers::SqlTypeHandle::concretizeSqlType<SqlIntType>(child.at(SpecialKey::CHILD_BTREE_KEY));
            SharedPagePtr childPage;
            readPage(node->get_the_entity()->getName(), childIndex, childPage);
            childPage->parentIndex = left->pageIndex;
            writePage(childPage);
        }

        writePage(left);
        writePage(node);
        return std::make_tuple(key, left, node);
    }
}
