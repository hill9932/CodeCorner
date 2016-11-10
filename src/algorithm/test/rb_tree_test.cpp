#include <gtest/gtest.h>
#include "../src/rb_tree.h"

using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_rb_tree_


TEST(TEST_CASE_NAME, CreateRBTree)
{
    typedef CRBTree<int, string> CTestRBTree;
    CTestRBTree tree;

    int values[] { 12, 13, 15, 10, 9, 200, 150, 131, -1, -4, 0, 44, 23, 20000, -14, 56, 8, 8, 120, 12};
    for (auto e : values)
        tree.addValue(e);


}