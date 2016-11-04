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
    CMyHeap     myTopNHeap;
    CMyTopHeap  myTopHeap;

    srand(time(NULL));
    for (int i = 0; i < HeapSize; ++i)
    {
        CMyHeap::NodeType node;
        node.key = rand();
        myHeap.addNode(node);
        myTopNHeap.addNode(node);
        myTopHeap.checkNode(node);
    }

    myHeap.sortHeap();
    myHeap.validate();

    myTopHeap.sortHeap();
    myTopHeap.validate();

    for (int i = 0; i < TopHeapSize; ++i)
    {
        CMyHeap::NodeType* node1 = myTopHeap.getNode(i);
        CMyHeap::NodeType* node2 = myHeap.getNode(HeapSize - TopHeapSize + i);
        ASSERT_EQ(node1->key, node2->key);
    }

    //
    // the top N min elements are in the front of the sorted heap
    //
    myTopNHeap.selectMinN(TopHeapSize);
    for (int i = 0; i < TopHeapSize; ++i)
    {
        int j = 0;
        CMyHeap::NodeType* node = myTopNHeap.getNode(i);
        cerr << node->key << ", ";

        for (j = 0; j < TopHeapSize; ++j)
        {
            CMyHeap::NodeType* checkNode = myHeap.getNode(j);
            if (node->key == checkNode->key)
                break;
        }
        ASSERT_TRUE(j < TopHeapSize);
    }
    cerr << endl;

    //
    // the top N max elements are in the top N heap
    //
    myTopNHeap.selectMaxN(TopHeapSize);
    for (int i = 0; i < TopHeapSize; ++i)
    {
        int j = 0;
        CMyHeap::NodeType* node = myTopNHeap.getNode(i);
        cerr << node->key << ", ";

        for (j = 0; j < TopHeapSize; ++j)
        {
            CMyHeap::NodeType* checkNode = myTopHeap.getNode(j);
            if (node->key == checkNode->key)
                break;
        }
        ASSERT_TRUE(j < TopHeapSize);
    }
    cerr << endl;

}
