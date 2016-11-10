#include <gtest/gtest.h>
#include "../src/bsearch_tree.h"


using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_bsearch_tree_


class CBSTreeTester : public testing::Test
{
protected:
    static CBSTree<float, int>    m_BSTree;
};

CBSTree<float, int> CBSTreeTester::m_BSTree;


TEST_F(CBSTreeTester, addValue)
{
    BSTreeNode<float, int>* node = NULL;

    ASSERT_EQ(0, m_BSTree.getNodeCount());
    ASSERT_EQ(0, m_BSTree.getFreqCount());

    //
    // add a root node
    //
    float value = 50;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq,   1);
    ASSERT_EQ(node->level,  0);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    ASSERT_EQ(1, m_BSTree.getNodeCount());
    ASSERT_EQ(1, m_BSTree.getFreqCount());

    //
    //     50
    //    /
    //   32
    //
    value = 32;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 1);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    ASSERT_EQ(2, m_BSTree.getNodeCount());
    ASSERT_EQ(2, m_BSTree.getFreqCount());

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 0);

    //
    //     50
    //    /   \
    //   32   101 
    //
    value = 101;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 1);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    ASSERT_EQ(3, m_BSTree.getNodeCount());
    ASSERT_EQ(3, m_BSTree.getFreqCount());

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 1);

    //
    //     50
    //    /   \
    //   32   101 
    //          \
    //          104
    //
    value = 104;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 2);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 2);

    //
    //     50
    //    /   \
    //   32   101 
    //          \
    //          104
    //          /
    //        102
    //
    value = 102;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 3);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 3);

    //
    //     50
    //    /   \
    //   32   101 
    //          \
    //          104
    //          /
    //        102
    //          \
    //          103
    //
    value = 103;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 4);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 4);

    //
    //     50
    //    /   \
    //   32   101 
    //          \
    //          104
    //          /  \
    //        102  111
    //          \
    //          103
    //
    value = 111;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 3);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 1);
    ASSERT_EQ(node->rHeight, 4);

    ASSERT_EQ(7, m_BSTree.getNodeCount());
    ASSERT_EQ(7, m_BSTree.getFreqCount());

    //
    //     50
    //    /   \
    //   32   101 
    //  /       \
    // 3        104
    //          /  \
    //        102  111
    //          \
    //          103
    //
    value = 3;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 2);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 2);
    ASSERT_EQ(node->rHeight, 4);


    //
    //     50
    //    /   \
    //   32   101 
    //  /    /  \
    // 3   70   104
    //          /  \
    //        102  111
    //          \
    //          103
    //
    value = 70;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 1);
    ASSERT_EQ(node->level, 2);
    ASSERT_EQ(node->lHeight, 0);
    ASSERT_EQ(node->rHeight, 0);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 2);
    ASSERT_EQ(node->rHeight, 4);


    value = 50;
    m_BSTree.addValue(value);
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->key, value);
    ASSERT_EQ(node->freq, 2);
    ASSERT_EQ(node->level, 0);
    ASSERT_EQ(node->lHeight, 2);
    ASSERT_EQ(node->rHeight, 4);

    ASSERT_EQ(9, m_BSTree.getNodeCount());
    ASSERT_EQ(10, m_BSTree.getFreqCount());

    /*
              50
        /            \
      32             101 
   /     \       /         \
  3      40    70          104
 /       /    /          /       \
1      39    60        102       111
            /           \           
          52.3          103           
    */
    value = 60;
    m_BSTree.addValue(value);
    ASSERT_EQ(10, m_BSTree.getNodeCount());
    ASSERT_EQ(11, m_BSTree.getFreqCount());

    value = 103;
    m_BSTree.addValue(value);
    ASSERT_EQ(10, m_BSTree.getNodeCount());
    ASSERT_EQ(12, m_BSTree.getFreqCount());

    value = 52.3;
    m_BSTree.addValue(value);
    ASSERT_EQ(11, m_BSTree.getNodeCount());
    ASSERT_EQ(13, m_BSTree.getFreqCount());

    value = 40;
    m_BSTree.addValue(value);
    ASSERT_EQ(12, m_BSTree.getNodeCount());
    ASSERT_EQ(14, m_BSTree.getFreqCount());

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 2);
    ASSERT_EQ(node->rHeight, 4);

    value = 39;
    m_BSTree.addValue(value);

    value = 1;
    m_BSTree.addValue(value);

    value = 50;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(node->lHeight, 3);
    ASSERT_EQ(node->rHeight, 4);

    ASSERT_EQ(14, m_BSTree.getNodeCount());
    ASSERT_EQ(16, m_BSTree.getFreqCount());

    m_BSTree.validate();
}


TEST_F(CBSTreeTester, delValue)
{
/*
              50
        /            \
      32             101 
   /     \       /         \
  3      40    70          104
 /       /    /          /       \
1      39    60        102       111
            /           \           
          52.3          103           
*/
    ASSERT_EQ(14, m_BSTree.getNodeCount());
    ASSERT_EQ(16, m_BSTree.getFreqCount());

    BSTreeNode<float, int>* node = NULL;
    int value = 200;
    ASSERT_EQ(-1, m_BSTree.delValue(value));

    value = 50;
    ASSERT_EQ(1, m_BSTree.delValue(value));
    ASSERT_EQ(0, m_BSTree.delValue(value));
    ASSERT_EQ(NULL, m_BSTree.findNode(value));
    ASSERT_EQ(13, m_BSTree.getNodeCount());
    ASSERT_EQ(14, m_BSTree.getFreqCount());

    value = 1;
    ASSERT_EQ(0, m_BSTree.delValue(value));
    ASSERT_EQ(NULL, m_BSTree.findNode(value));

    value = 3;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(NULL, node->left);

    value = 101;
    ASSERT_EQ(0, m_BSTree.delValue(value));
    ASSERT_EQ(NULL, m_BSTree.findNode(value));

    value = 104;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(70, node->parent->key);

    value = 32;
    ASSERT_EQ(0, m_BSTree.delValue(value));
    ASSERT_EQ(NULL, m_BSTree.findNode(value));

    value = 111;
    ASSERT_EQ(0, m_BSTree.delValue(value));
    ASSERT_EQ(NULL, m_BSTree.findNode(value));

    ASSERT_EQ(9, m_BSTree.getNodeCount());
    ASSERT_EQ(10, m_BSTree.getFreqCount());

/*
              40
        /            \
      39             70 
   /             /         \
  3            60          104
              /          /       
             52.3      102       
                        \           
                        103           
*/
    value = 104;
    ASSERT_EQ(0, m_BSTree.delValue(value));
    value = 103;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(102, node->left->key);

    value = 102;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(103, node->parent->key);

    value = 102;
    m_BSTree.delValue(value);

    value = 103;
    node = m_BSTree.findNode(value);
    ASSERT_EQ(NULL, node->left);
    ASSERT_EQ(NULL, node->right);
    m_BSTree.removeValue(value);
    ASSERT_EQ(6, m_BSTree.getNodeCount());
    ASSERT_EQ(6, m_BSTree.getFreqCount());

    m_BSTree.addValue(80);
    m_BSTree.addValue(100);

/*
              40
        /            \
      39             70 
   /             /         \
  3            60          80
              /               \       
             52.3             100       
*/
    m_BSTree.delValue(60);
    m_BSTree.delValue(52.3);
    node = m_BSTree.findNode(60);
    ASSERT_EQ(NULL, node);

    m_BSTree.delValue(40);
    node = m_BSTree.findNode(70);
    ASSERT_EQ(39, node->parent->key);

    m_BSTree.delValue(39);
    m_BSTree.delValue(3);
    m_BSTree.delValue(70);
    node = m_BSTree.findNode(80);
    ASSERT_EQ(NULL, node->parent);

    m_BSTree.delValue(80);
    node = m_BSTree.findNode(100);
    ASSERT_EQ(NULL, node->parent);

    m_BSTree.delValue(100);
    ASSERT_EQ(0, m_BSTree.getNodeCount());
    ASSERT_EQ(0, m_BSTree.getFreqCount());
}