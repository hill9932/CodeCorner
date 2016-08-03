#include <gtest/gtest.h>
#include "../src/file_util.h"

using namespace LabSpace::Common;

#define TEST_CASE_NAME  Test_file_

TEST(TEST_CASE_NAME, GetAppName)
{
    EXPECT_STREQ("common_test", FileUtil::GetAppName().c_str());
}
