#include <gtest/gtest.h>
#include "../src/hash_map.h"

using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_hash_map_


TEST(TEST_CASE_NAME, CreateHeapMap)
{
    CHashMap<string> hashMap;

    string value = "123_abc";
    for (int i = 1; i <= 10; ++i)
        ASSERT_EQ(i, hashMap.addNode(value));

    ASSERT_EQ(10, hashMap.getNodesCount());

    value += " ";
    for (int i = 1; i <= 10; ++i)
        ASSERT_EQ(i, hashMap.addNode(value));

    ASSERT_EQ(20, hashMap.getNodesCount());

    value += " ";
    for (int i = 1; i <= 10; ++i)
        ASSERT_EQ(i, hashMap.addNode(value));

    ASSERT_EQ(30, hashMap.getNodesCount());

    for (int i = 9; i >= 0; --i)
        ASSERT_EQ(i, hashMap.delNode(value));

    ASSERT_EQ(20, hashMap.getNodesCount());

    ASSERT_EQ(-1, hashMap.removeNode(value));
    ASSERT_EQ(NULL, hashMap.findNode(value));

    value += "!@34";
    for (int i = 1; i <= 10; ++i)
        ASSERT_EQ(i, hashMap.addNode(value));

    ASSERT_EQ(30, hashMap.getNodesCount());

    ASSERT_EQ(0, hashMap.removeNode(value));
    ASSERT_EQ(-1, hashMap.delNode(value));
    ASSERT_EQ(20, hashMap.getNodesCount());
}



