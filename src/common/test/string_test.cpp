#include <gtest/gtest.h>
#include "../src/string_util.h"

using namespace LabSpace::Common;

#define TEST_CASE_NAME  Test_string_


void CheckStrSplit(const tstring& _src, int _expected)
{
    vector<tstring> v;
    tstring tokens = " \t\n\\:,";
    int n = StrUtil::StrSplit(_src, tokens, v);
    EXPECT_EQ(n, _expected);
}

TEST(TEST_CASE_NAME, StrSplit)
{
    //
    // the source has multiple split tokens
    //    
    tstring src = "abc def \t123\t456\\789:ghi,xyz";
    tstring tokens = " \t\n\\:,";
    CheckStrSplit(src, 7);

    //
    // the source has no split tokens
    //
    src.assign(100, 'a');
    CheckStrSplit(src, 1);

    //
    // the source is full of blanks 
    //
    src.assign(5, ' ');
    CheckStrSplit(src, 0);

    //
    // the source is full of split tokens
    //
    src.assign(tokens.begin(), tokens.end());
    CheckStrSplit(src, 0);

    //
    // the source is begin with split tokens and end with split tokens
    //
    src.append("abc def \t123\t456\\789:ghi,xyz");
    src.append(tokens.begin(), tokens.end());
    CheckStrSplit(src, 7);
}


void CheckStrSubSplit(const tstring& _src, const tstring& _token, int _expected)
{
    vector<tstring> v;
    int n = StrUtil::StrSubSplit(_src, _token, v);
    EXPECT_EQ(n, _expected);
}

TEST(TEST_CASE_NAME, StrSubSplit)
{
    //
    // the split token has only on char
    //
    tstring src = "abc\tdef\t123\t456\t789\tghi\txyz";
    tstring tokens = "\t";
    CheckStrSubSplit(src, tokens, 7);

    //
    // the split token has multiple chars
    //
    src = "abc\r\ndef\r\n123\r\n456\r\n789\r\nghi\r\nxyz";
    tokens = "\r\n";
    CheckStrSubSplit(src, tokens, 7);

    //
    // the source is full of split tokens
    //
    src = "\r\n\r\n\r\n\r\n\r\n\r\n";
    tokens = "\r\n";
    CheckStrSubSplit(src, tokens, 0);

    //
    // the source begins with split tokens and end with split tokens
    //
    src = "\r\n\r\nabc\r\ndef\r\nghi\r\n";
    tokens = "\r\n";
    CheckStrSubSplit(src, tokens, 3);

    //
    // the source has no split tokens
    //
    src.assign(100, 'a');
    tokens = "\r\n";
    CheckStrSubSplit(src, tokens, 1);

    src.assign(100, '\r');
    tokens = "\r\n";
    CheckStrSubSplit(src, tokens, 1);
}


TEST(TEST_CASE_NAME, StrStrim)
{
    //
    // src begins with space tokens and ends with space tokens
    //
    tstring src;
    tstring spaceToken = " \t\n\r\v\f";
    tstring value = "abc 123 xyz #$%^&*()_+";
    src.assign(spaceToken.begin(), spaceToken.end());
    src.append(value.begin(), value.end());
    src.append(spaceToken.begin(), spaceToken.end());

    tstring result = StrUtil::StrTrim(src);
    EXPECT_TRUE(result == value);

    //
    // src has space tokens in the middle
    //
    src.assign(spaceToken.begin(), spaceToken.end());
    src.append(value.begin(), value.end());
    src.append(spaceToken.begin(), spaceToken.end());
    src.append(value.begin(), value.end());
    src.append(spaceToken.begin(), spaceToken.end());
    result = StrUtil::StrTrim(src);
    EXPECT_TRUE(result == value + spaceToken + value);
}


TEST(TEST_CASE_NAME, StrReplace)
{
    tstring src;
    tstring oldTokens = "abc";
    tchar   newToken = TEXT('1');
    src.assign(oldTokens.begin(), oldTokens.end());

    string result = StrUtil::StrReplace(src, oldTokens, newToken);
    EXPECT_TRUE(result == "111");
    
    src = "abc1234abc";
    result = StrUtil::StrReplace(src, oldTokens, newToken);
    EXPECT_TRUE(result == "1111234111");
}


TEST(TEST_CASE_NAME, Str2Argv)
{
    //
    // command has several parameters split by blank
    //
    tchar* cmd = "NetKeeper.exe --run-mode=2 --print-console=1 --test-capture=0x100";
    tchar* argv[20] = { 0 };
    int argc = 20;

    StrUtil::Str2Argv(cmd, argv, argc);
    EXPECT_TRUE(argc == 4);

    //
    // command has several parameters split by multiple blanks and some parameter has ''
    //
    argc = 20;
    cmd = "NetKeeper.exe --run-mode=2   --print-console=1   --test-capture=0x100  --config-file='/pvc/data/NetKeeper.lua' --agent-name=\"default\"";
    StrUtil::Str2Argv(cmd, argv, argc);
    EXPECT_TRUE(argc == 6);

    //
    // there are some blanks before and after the command line
    //
    argc = 20;
    cmd = "  NetKeeper.exe --run-mode=2   --print-console=1   --test-capture=0x100  --config-file='/pvc/data/NetKeeper.lua\" --agent-name=\"default\""  ;
    StrUtil::Str2Argv(cmd, argv, argc);
    EXPECT_TRUE(argc == 6);
}


TEST(TEST_CASE_NAME, Bin2Str)
{
    const byte bin[] = {0x11, 0x12, 0x23, 0x00, 0x04, 0xa0, 0xA1, 0xBA, 0xFE, 0x00};
    std::shared_ptr<tchar> result = StrUtil::Bin2Str(bin, sizeof(bin) / sizeof(byte));
    EXPECT_STRCASEEQ(result.get(), TEXT("1112230004a0a1bafe00"));

    const byte bin2[] = {1, 4, 7, 10, 12, 15};
    result = StrUtil::Bin2Str(bin2, sizeof(bin2) / sizeof(byte));
    EXPECT_STRCASEEQ(result.get(), TEXT("0104070a0c0f"));
}


TEST(TEST_CASE_NAME, GetFileName)
{
    //
    // the full path is relative
    //
    tstring cmd = "  ./NetKeeper.exe --run-mode=2   --print-console=1   --test-capture=0x100  --agent-name=\"default\"";
    tstring result = StrUtil::GetFileName(cmd);
    EXPECT_TRUE(result == "NetKeeper.exe");

    //
    // the full path is absolutely
    //
    cmd = "/usr/local/bin/NetKeeper --run-mode=1";
    result = StrUtil::GetFileName(cmd);
    EXPECT_TRUE(result == "NetKeeper");

    //
    // no path
    //
    cmd = ".NetKeeper --run-mode=1";
    result = StrUtil::GetFileName(cmd);
    EXPECT_TRUE(result == ".NetKeeper");

    cmd = "./NetKeeper.exe";
    result = StrUtil::GetFileName(cmd);
    EXPECT_TRUE(result == "NetKeeper.exe");

    cmd = "NetKeeper.exe";
    result = StrUtil::GetFileName(cmd);
    EXPECT_TRUE(result == "NetKeeper.exe");
}


TEST(TEST_CASE_NAME, GetFilePath)
{
    //
    // the full path is relative
    //
    tstring cmd = "  ./NetKeeper.exe --run-mode=2   --print-console=1   --test-capture=0x100  --agent-name=\"default\"";
    tstring result = StrUtil::GetFilePath(cmd);
    EXPECT_TRUE(result == "./NetKeeper.exe");

    //
    // the full path is absolutely
    //
    cmd = "/usr/local/bin/NetKeeper --run-mode=1";
    result = StrUtil::GetFilePath(cmd);
    EXPECT_TRUE(result == "/usr/local/bin/NetKeeper");

    //
    // no path
    //
    cmd = ".NetKeeper --run-mode=1";
    result = StrUtil::GetFilePath(cmd);
    EXPECT_TRUE(result == ".NetKeeper");

    cmd = "./NetKeeper.exe";
    result = StrUtil::GetFilePath(cmd);
    EXPECT_TRUE(result == "./NetKeeper.exe");

    cmd = "NetKeeper.exe";
    result = StrUtil::GetFilePath(cmd);
    EXPECT_TRUE(result == "NetKeeper.exe");
}

TEST(TEST_CASE_NAME, IsMatch)
{
    tstring src = "123456";
    tstring match = "^%d+$";
    EXPECT_TRUE(StrUtil::IsMatch(src, match));
    EXPECT_TRUE(StrUtil::IsAllNumber(src));

    src = "123a456";
    EXPECT_FALSE(StrUtil::IsMatch(src, match));
    EXPECT_FALSE(StrUtil::IsAllNumber(src));

    EXPECT_TRUE(StrUtil::IsValidPath("c:/abc/123"));
    EXPECT_TRUE(StrUtil::IsValidPath("c:\\abc/123_efze"));
    EXPECT_FALSE(StrUtil::IsValidPath("c:\\abc/123_efze#"));
}

