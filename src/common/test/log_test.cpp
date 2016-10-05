#include <gtest/gtest.h>
#include "../src/file_log.h"
#include "../src/lua_.h"
#include "../src/file_util.h"


#define TEST_CASE_NAME  Test_log_


class CLogTester : public testing::Test
{
public:
    static void SetUpTestCase()
    {
#ifdef WIN32
        WORD wVersionRequested;
        WSADATA wsaData;
        wVersionRequested = MAKEWORD(2, 2);

        int err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0)
            throw "Fail to call WSAStartup";
#endif

        int n = s_lua.regFunc("Entry", LuaCallbackFunction);
        EXPECT_EQ(0, n);

        CFileLogger::initStatic();
    }

    static void TearDownTestCase()
    {
#ifdef WIN32
        WSACleanup();
#endif
    }

    static int LuaCallbackFunction(lua_State *L);

protected:
    static CLua         s_lua;
    static int          s_entries;
    static tstring      s_fileName;
    CFileLogger         m_logger;
};


int     CLogTester::s_entries = 0;
CLua    CLogTester::s_lua;
tstring CLogTester::s_fileName;

int CLogTester::LuaCallbackFunction(lua_State *L)
{
    ++s_entries;
    return 0;
}

#define LOG_COUNT 100000

void AnotherThread()
{
    for (int i = 0; i < LOG_COUNT; ++i)
    {
        LOG_INFO_STREAM << i << ". I'm another thread!";
        LOG_INFO_STREAM << i << ". " << 3.1415926;
    }
}

TEST_F(CLogTester, logFile)
{
    try
    {
        ASSERT_TRUE(m_logger.start());

        std::thread td(AnotherThread);

        for (int i = 0; i < LOG_COUNT; ++i)
        {
            LOG_INFO_STREAM << i << ". Hello, mylog!";
            LOG_INFO_STREAM << i << ". " << (u_int64)123456789LL;
            LOG_INFO_STREAM << i << ". " << &td;
        }

        td.join();

        m_logger.stop();
        m_logger.join();

        s_fileName = m_logger.getFileName();
    }
    catch (...)
    {
        EXPECT_TRUE(0);
    }
}

TEST_F(CLogTester, checkLogFile)
{
    int n = s_lua.doFile(s_fileName);
    ASSERT_EQ(n, 0);
    EXPECT_EQ(5 * LOG_COUNT, s_entries);

    FileUtil::DeletePath(s_fileName);
    EXPECT_FALSE(FileUtil::IsFileExist(s_fileName));
}