//
// Created by chiendd on 04/08/2024.
//

#include <gtest/gtest.h>

#include "../src/core/storage/lru_cache/lru_cache.h"

using namespace Csql;

LRUCache<int, int*>* initStandardLRUCache(size_t maxSize) {
    LRUCache<int, int*>* lru_cache = new LRUCache<int, int*>(maxSize);
    return lru_cache;
}

TEST(LRU_CACHE_TEST, PUT_GET_METHOD_TEST) {
    size_t maxSize = 2;
    LRUCache<int, int*>* lru_cache = initStandardLRUCache(maxSize);

    int *value1 = new int(12);
    int *value2 = new int(22);
    int *value3 = new int(32);

    const int key1 = 5;
    const int key2 = 19;
    const int key3 = 24;

    lru_cache->put(key1, value1);
    lru_cache->put(key2, value2);
    EXPECT_EQ(lru_cache->contains(key1), true);
    EXPECT_EQ(lru_cache->contains(key2), true);
    EXPECT_EQ(*lru_cache->get(key1), *value1);
    EXPECT_EQ(*lru_cache->get(key2), *value2);

    int *removeValue = lru_cache->put(key3, value3);
    EXPECT_EQ(*removeValue, *value1);
    EXPECT_EQ(lru_cache->size(), maxSize);
    EXPECT_EQ(lru_cache->contains(key1), false);
    EXPECT_EQ(lru_cache->contains(key3), true);
    EXPECT_EQ(*lru_cache->get(key3), *value3);
}
