//
// Created by chiendd on 04/08/2024.
//

#ifndef LRU_CACHE_H
#define LRU_CACHE_H
#include <functional>
#include <unordered_map>
#include "linked_list.h"
#include "../page/slotted_page.h"

namespace Csql {
    using CacheVisitor = std::function<void(SharedPagePtr aPage)>;

    template<typename KeyT, typename ValueT>
    class LRUCache {
    protected:
        std::unordered_map<KeyT, Node<KeyT, ValueT>*> lookup;
        LinkedList<KeyT, ValueT> linkedList;
        size_t maxSize;
    public:
        LRUCache(uint32_t maxSize) : maxSize(maxSize) {};

        ValueT put(const KeyT &aKey, const ValueT &aValue) {
            ValueT theValueReturn = aValue;
            if (size() >= maxSize) {
                auto thePair = linkedList.pop();
                lookup.erase(thePair.first);
                theValueReturn = thePair.second;
            }
            lookup[aKey] = linkedList.push(aKey, aValue);
            return theValueReturn;
        }

        ValueT get(const KeyT &aKey) {
            if (!lookup.contains(aKey)) {
                return nullptr;
            }
            Node<KeyT, ValueT> *theNode = lookup[aKey];
            linkedList.pushToEnd(theNode);
            return theNode->value;
        }

        bool contains(const KeyT &aKey) const {
            return lookup.contains(aKey);
        }

        size_t size() {
            return lookup.size();
        }

        void flush(const CacheVisitor &aVisitor) {
            for (Node<KeyT, ValueT> *iterNode = linkedList.begin->next; iterNode != linkedList.end; iterNode = iterNode->next) {
                aVisitor(iterNode->value);
            }

            linkedList.clear();
        }
    };
}



#endif //LRU_CACHE_H
