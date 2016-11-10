#include <gtest/gtest.h>
#include "../src/sort_array.h"

using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_sort_array


TEST(TEST_CASE_NAME, CreateSortArray)
{
    CSortArray<int, 111>  sortArray;
    ASSERT_TRUE(sortArray.end() == sortArray.npos);

    int values[] { 12, 13, 15, 10, 9, 200, 150, 131, -1, -4, 0, 44, 23, 20000, -14, 56, 8, 8, 120, 12};
    for (auto e: values)
        sortArray.addValue(e);

    sortArray.cycle(3);
    sortArray.cycle(1);
    sortArray.cycle(0);

    sortArray.cycle(-1);
    sortArray.cycle(-2);
    sortArray.cycle(-1);

    for (int i = 0; i < (sizeof(values) / sizeof(values[0])) - 1; ++i)
        ASSERT_EQ(sortArray[i], values[i]);

    sortArray.sort();
    for (int i = 0; i < (sizeof(values) / sizeof(values[0])) - 1; ++i)
        ASSERT_TRUE(sortArray[i] <= sortArray[i + 1]);
}
