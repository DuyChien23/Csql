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

void Storage::writePage(SharedPagePtr &aPage) {
    if (aPage == nullptr || !aPage->pageIndex) return;
    aPage->save();
    SharedPagePtr needWritingPage = aPage;
    if (Configs::cacheMaxSize) {
        std::string theKey = createKey(aPage->get_the_entity()->getName(), aPage->pageIndex);
        needWritingPage = pageCache->put(theKey, aPage);
    }
    writePageUncache(needWritingPage);
}

void Storage::writePageUncache(SharedPagePtr &aPage) {
    std::fstream &f = getFstream(aPage->get_the_entity()->getName());
    f.seekg(aPage->pageIndex * Configs::storageUnitSize);
    aPage->encode(f);
}

void Storage::readPage(const std::string &entityName, uint32_t pageIndex, SharedPagePtr &aPage) {
    if (!pageIndex) {
        aPage = nullptr;
        return;
    }

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

void Storage::readPageUncache(const std::string &entityName, uint32_t pageIndex, SharedPagePtr &aPage) {
    std::fstream &f = getFstream(entityName);
    f.seekg(pageIndex * Configs::storageUnitSize);
    auto temp = std::make_shared<SlottedPage>(pageIndex, getEntity(entityName));
    temp->decode(f);
    temp->loadType();
    if (temp->pageType == PageType::leaf_b_plus_node) {
        aPage = std::make_shared<LeafBPlusNode>(*temp);
    } else if (aPage->pageType == PageType::internal_b_plus_node) {
        aPage = std::make_shared<InternalBPlusNode>(*temp);
    } else {
        aPage = std::make_shared<FreePage>(*temp);
    }
}

void Storage::deleteEntityFile(const std::string &entityName) {
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

void Storage::eachEntity(const EntityVisitor &entityVisitor) {
    for (auto &entityName: *getListEntityNames()) {
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

SharedPagePtr Storage::addPageIntoFreeZone(SharedPagePtr &aPage) {
    SharedEntityPtr theEntity = aPage->get_the_entity();
    SharedPagePtr thePage = std::make_shared<FreePage>(*aPage);
    aPage = nullptr;
    dynamic_cast<FreePage *>(thePage.get())->setNextFreePage(theEntity->getFirstFreePage());
    theEntity->setFirstFreePage(thePage->pageIndex);
    writePage(thePage);
    return thePage;
}

//===========================================B+Tree=Interface=========================================
void Storage::setBTree(IndexingMetadata &indexingMetadata, Tuple tuple) {
    SharedPagePtr leaf;
    if (indexingMetadata.root == 0) {
        leaf = std::make_shared<LeafBPlusNode>(*popFreePage(indexingMetadata.entityName));
        indexingMetadata.root = leaf->pageIndex;
    } else {
        leaf = findLeaf(indexingMetadata, GET_BTREE_KEY(tuple));
    }

    leaf->addTuple(tuple);
    if (leaf->getSizeOfUsedBytes() > Configs::storageMaximumUsedBytes) {
        insertBtree(indexingMetadata, splitLeaf(leaf));
    } else {
        writePage(leaf);
    }
}

Tuple Storage::removeBtree(IndexingMetadata &indexingMetadata, const BPlusKey &key, SharedPagePtr node) {
    if (!node) {
        node = findLeaf(indexingMetadata, key);
    }

    Tuple result;

    if (node->pageType == PageType::leaf_b_plus_node) {
        result = removeFromLeaf(key, node);
    } else {
        removeFromInternal(key, node);
    }

    SharedPagePtr parent;
    readPage(indexingMetadata.entityName, node->parentIndex, parent);

    if (node->getSizeOfUsedBytes() < Configs::storageMinimumUsedBytes) {
        if (node->pageIndex == indexingMetadata.root) {
            if (node->pageType == PageType::internal_b_plus_node && node->get_tuples().size() == 1) {
                indexingMetadata.root = Helpers::TupleHandle::getBtreeChild(*node->get_tuples().begin());
                addPageIntoFreeZone(node);

                SharedPagePtr root;
                readPage(indexingMetadata.entityName, indexingMetadata.root, root);
                root->parentIndex = 0;
                writePage(root);
            } else {
                writePage(node);
            }
            return result;
        }

        if (node->pageType == PageType::leaf_b_plus_node) {
            SharedPagePtr prev;
            SharedPagePtr next;

            readPage(indexingMetadata.entityName, node->prevLeaf, prev);
            readPage(indexingMetadata.entityName, node->nextLeaf, next);

            auto updatePrevIndexOfNextNode = [&](SharedPagePtr &aNode) {
                SharedPagePtr temp;
                readPage(indexingMetadata.entityName, aNode->nextLeaf, temp);
                if (aNode->nextLeaf) {
                    temp->prevLeaf = node->pageIndex;
                    writePage(temp);
                }
            };

            if (next && node->parentIndex == next->parentIndex && next->getSizeOfUsedBytes() >
                Configs::storageMinimumUsedBytes) {
                LEAF_CAST(node)->borrowKeyFromRightLeaf(next, parent);
                writePage(next);
            } else if (prev && node->parentIndex == prev->parentIndex && prev->getSizeOfUsedBytes() >
                       Configs::storageMinimumUsedBytes) {
                LEAF_CAST(node)->borrowKeyFromLeftLeaf(prev, parent);
                writePage(prev);
            } else if (next && node->parentIndex == next->parentIndex && next->getSizeOfUsedBytes() <=
                       Configs::storageMinimumUsedBytes) {
                LEAF_CAST(node)->mergeLeafNode(next, parent);
                updatePrevIndexOfNextNode(node);
                addPageIntoFreeZone(next);
            } else if (prev && node->parentIndex == prev->parentIndex && prev->getSizeOfUsedBytes() <=
                       Configs::storageMinimumUsedBytes) {
                LEAF_CAST(prev)->mergeLeafNode(node, parent);
                updatePrevIndexOfNextNode(prev);
                addPageIntoFreeZone(node);
                node = prev;
            }
        } else {
            int index = parent->lower_bound(node->get_btree_key(), 1);
            if (index == -1) {
                index = 0;
            }

            SharedPagePtr prev = nullptr;
            SharedPagePtr next = nullptr;

            if (index + 1 < parent->get_tuples().size()) {
                readPage(indexingMetadata.entityName, parent->get_child_index(index + 1), next);
            }

            if (index > 0) {
                readPage(indexingMetadata.entityName, parent->get_child_index(index - 1), prev);
            }

            auto updateParentIndexOfChild = [&](SharedPagePtr &aNode, int num, bool runningFromBegin) {
                auto run = [&](const Tuple &tuple) {
                    SharedPagePtr temp;
                    readPage(indexingMetadata.entityName, Helpers::TupleHandle::getBtreeChild(tuple), temp);
                    temp->parentIndex = aNode->pageIndex;
                    writePage(temp);
                };


                for (int i = 0; i < num; ++i) {
                    int index = runningFromBegin ? i : aNode->get_num_slots() - i - 1;
                    run(aNode->getTuple(index));
                }
            };

            if (next && next->getSizeOfUsedBytes() > Configs::storageMinimumUsedBytes) {
                INTERNAL_CAST(node)->borrowKeyFromRightInternal(next, parent, index);
                updateParentIndexOfChild(node, 1, false);
                writePage(next);
            } else if (prev && prev->getSizeOfUsedBytes() > Configs::storageMinimumUsedBytes) {
                INTERNAL_CAST(node)->borrowKeyFromLeftInternal(prev, parent, index);
                updateParentIndexOfChild(node, 1, true);
                writePage(prev);
            } else if (next && next->getSizeOfUsedBytes() <= Configs::storageMinimumUsedBytes) {
                int num = next->get_tuples().size();
                INTERNAL_CAST(node)->mergeInternalNode(next, parent, index);
                updateParentIndexOfChild(node, num, false);
                addPageIntoFreeZone(next);
            } else if (prev && prev->getSizeOfUsedBytes() <= Configs::storageMinimumUsedBytes) {
                int num = node->get_tuples().size();
                INTERNAL_CAST(prev)->mergeInternalNode(node, parent, index - 1);
                updateParentIndexOfChild(prev, num, false);
                addPageIntoFreeZone(node);
                node = prev;
            }
        }
    }

    writePage(node);

    if (parent) {
        removeBtree(indexingMetadata, key, parent);
    }
    return result;
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

BtreeLeafIterator Storage::searchBtree(const IndexingMetadata &indexingMetadata, BPlusKey &key) {
    if (!indexingMetadata.root) return endLeaf();
    SharedPagePtr node = findLeaf(indexingMetadata, key);
    auto index = node->lower_bound(key, 0);
    if (index == -1 || GET_BTREE_KEY(node->getTuple(index)) != key) {
        return endLeaf();
    }
    return BtreeLeafIterator(node->lower_bound(key, 0), node);
}

BtreeLeafIterator Storage::beginLeaf(const IndexingMetadata &indexingMetadata) {
    return beginLeaf(indexingMetadata.entityName, indexingMetadata.root);
}

BtreeLeafIterator Storage::beginLeaf(const std::string &entityName, uint32_t nodeIndex) {
    if (!nodeIndex) return endLeaf();

    while (true) {
        SharedPagePtr node;
        readPage(entityName, nodeIndex, node);

        if (node->pageType == PageType::leaf_b_plus_node) {
            if (node->get_tuples().empty()) {
                return endLeaf();
            }
            return BtreeLeafIterator(0, node);
        }

        nodeIndex = Helpers::SqlTypeHandle::concretizeSqlType<SqlIntType>(
            node->get_tuples().begin()->at(SpecialKey::CHILD_BTREE_KEY));
    }
}

BtreeLeafIterator &Storage::endLeaf() {
    return endLeafBnode;
}

BtreeLeafIterator &Storage::nextLeaf(BtreeLeafIterator &iter) {
    iter.index++;
    if (iter.index >= iter.page->get_num_slots()) {
        iter.index = 0;
        int nextIndex = dynamic_cast<LeafBPlusNode *>(iter.page.get())->nextLeaf;
        if (nextIndex == 0) {
            return iter = endLeaf();
        }
        readPage(iter.page->get_the_entity()->getName(), nextIndex, iter.page);
    }
    return iter;
}

void Storage::printBtree(IndexingMetadata &indexingMetadata, int nodeIndex, std::string _prefix, bool _last) {
    if (nodeIndex == 0) {
        std::cerr << "====================B+Tree====================" << std::endl;
        nodeIndex = indexingMetadata.root;
    }
    std::cerr << _prefix << "├ [";

    SharedPagePtr node;
    readPage(indexingMetadata.entityName, nodeIndex, node);

    for (int i = (node->pageType == PageType::internal_b_plus_node); i < node->get_num_slots(); ++i) {
        Helpers::TupleHandle::getBTreeKey(node->getTuple(i)).log();
        if (i != node->get_num_slots() - 1) {
            std::cerr << ", ";
        }
    }

    std::cerr << "~[PageIndex=" << node->pageIndex << "][ParentIndex=" << node->parentIndex << "]" << std::endl;

    _prefix += _last ? "   " : "╎  ";

    if (node->pageType == PageType::internal_b_plus_node) {
        for (int i = 0; i < node->get_num_slots(); ++i) {
            bool _last = (i == node->get_num_slots() - 1);
            printBtree(indexingMetadata, Helpers::TupleHandle::getBtreeChild(node->getTuple(i)), _prefix, _last);
        }
    }

    std::cerr.flush();
}

//===========================================B+Tree=Internal=========================================
void Storage::insertBtree(IndexingMetadata &indexingMetadata, SplitNodeType result) {
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

    if (parent->getSizeOfUsedBytes() > Configs::storageMinimumUsedBytes) {
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

    for (auto &child: left->get_tuples()) {
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

Tuple Storage::removeFromLeaf(const BPlusKey &key, SharedPagePtr node) {
    int index = node->lower_bound(key, 0);
    if (index == -1) {
        throw Errors("Key not found");
    }
    Tuple result = node->getTuple(index);
    node->deleteTuple(index);
    if (node->parentIndex) {
        SharedPagePtr parent;
        readPage(node->get_the_entity()->getName(), node->parentIndex, parent);
        int indexInParent = INTERNAL_CAST(parent)->indexOfChild(key);
        if (indexInParent) {
            parent->getTuple(indexInParent).at(SpecialKey::BTREE_KEY) = node->get_btree_key();
            writePage(parent);
        }
    }
    return result;
}

void Storage::removeFromInternal(const BPlusKey &key, SharedPagePtr node) {
    int index = node->lower_bound(key, 1);
    if (index != -1) {
        uint32_t childIndex = Helpers::TupleHandle::getBtreeChild(node->getTuple(index));
        auto leftMostLeaf = beginLeaf(node->get_the_entity()->getName(), childIndex).page;
        node->getTuple(index).at(SpecialKey::BTREE_KEY) = leftMostLeaf->get_btree_key();
    }
}
