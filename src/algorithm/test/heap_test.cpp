#include <gtest/gtest.h>
#include "../src/min_max_heap.h"

using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_heap_


TEST(TEST_CASE_NAME, CreateHeap)
{
    const int HeapSize = 1000;
    typedef CHeap<int, string, HeapSize> CMyHeap;
    CMyHeap myHeap;
    
    int maxKey = 0;
    srand(time(NULL));
    for (int i = 0; i < HeapSize; ++i)
    {
        CMyHeap::NodeType node;
        node.key = rand();
        if (node.key > maxKey)
            maxKey = node.key;

        myHeap.addNode(node);
    }
    ASSERT_EQ(HeapSize, myHeap.getNodesCount());

   // myHeap.validate();
    myHeap.createMaxHeap();

    CMyHeap::NodeType* nodes = myHeap.getNodes();
    for (int i = 0; i < HeapSize / 2 - 1; ++i)
    {
        ASSERT_TRUE(nodes[i].key >= nodes[2 * i + 1].key);
        ASSERT_TRUE(nodes[i].key >= nodes[2 * i + 2].key);
    }

   // myHeap.validate();
    myHeap.sortHeap();
   // myHeap.validate();

    for (int i = 0; i < HeapSize - 1; ++i)
    {
        ASSERT_TRUE(nodes[i].key <= nodes[i + 1].key);
    }
}


TEST(TEST_CASE_NAME, TopNHeap)
{
    const int HeapSize = 1000;
    const int TopHeapSize = 13;
    typedef CHeap<int, string, HeapSize> CMyHeap;
    typedef CTopNHeap<int, string, TopHeapSize> CMyTopHeap;

    CMyHeap     myHeap;
    CMyTopHeap  myTopHeap;

    srand(time(NULL));
    for (int i = 0; i < HeapSize; ++i)
    {
        CMyHeap::NodeType node;
        node.key = rand();
        myHeap.addNode(node);
        myTopHeap.checkNode(node);
    }

    myHeap.sortHeap();
//    myHeap.validate();

    myTopHeap.sortHeap();
//    myTopHeap.validate();

    for (int i = 0; i < TopHeapSize; ++i)
    {
        CMyHeap::NodeType* node1 = myTopHeap.getNode(i);
        CMyHeap::NodeType* node2 = myHeap.getNode(HeapSize - TopHeapSize + i);
        ASSERT_EQ(node1->key, node2->key);
    }
}
