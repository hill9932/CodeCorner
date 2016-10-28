#include <gtest/gtest.h>
#include "../src/trie_tree.h"


using namespace LabSpace::algorithm;

#define TEST_CASE_NAME  Test_trie_tree_


class CTrieTreeTester : public testing::Test
{
protected:
    static CUrlTrieTree    m_urlTree;
};

CUrlTrieTree CTrieTreeTester::m_urlTree;


TEST_F(CTrieTreeTester, addUrl)
{
    const tchar* url = "http://121.42.136.36/redmine/issues/1962";
    const tchar* url2 = "http://121.42.136.36/redmine/issues/1963";     // diff the last char
    const tchar* url3 = "http://121.42.136.36/redmine/issues/1962/1";   // more chars at the end
    const tchar* url4 = "http:/121.42.136.36/redmine/issues/1962";      // diff one char in the middle

    int n = m_urlTree.addUrl(url);
    EXPECT_EQ(1, n);
    EXPECT_EQ(1, m_urlTree.getAllUrlCount());
    EXPECT_EQ(1, m_urlTree.getAllFreqCount());

    n = m_urlTree.getUrlCount(url);
    EXPECT_EQ(1, n);

    n = m_urlTree.addUrl(url);
    EXPECT_EQ(2, n);
    EXPECT_EQ(1, m_urlTree.getAllUrlCount());
    EXPECT_EQ(2, m_urlTree.getAllFreqCount());

    n = m_urlTree.getUrlCount(url);
    EXPECT_EQ(2, n);

    m_urlTree.delUrl(url);
    n = m_urlTree.getUrlCount(url);
    EXPECT_EQ(0, n);
    EXPECT_EQ(0, m_urlTree.getAllUrlCount());
    EXPECT_EQ(0, m_urlTree.getAllFreqCount());

    n = m_urlTree.getUrlCount(url2);
    EXPECT_EQ(0, n);

    n = m_urlTree.getUrlCount(url3);
    EXPECT_EQ(0, n);

    n = m_urlTree.getUrlCount(url4);
    EXPECT_EQ(0, n);

    const tchar* urls[] = {
        "http://121.42.136.36/redmine/issues/1963",         // 0
        "https://121.42.136.36/redmine/issues/1963",        // 1
        "https://122.42.136.36/redmine/issues/1963",        // 2
        "https://121.42.136.36/redmine2/issues/1963",       // 3
        "https://121.42.136.36/redmine/issues/1964",        // 4
        "https://121.42.136.36/redmine/issues/1964/1",      // 5
        "file://121.42.136.36/redmine/issues/1963"
    };

    for (int i = 0; i < sizeof(urls) / sizeof(urls[0]); ++i)
    {
        n = m_urlTree.addUrl(urls[i]);
        EXPECT_EQ(1, n);
        EXPECT_EQ(1, m_urlTree.getAllUrlCount());
        EXPECT_EQ(1, m_urlTree.getAllFreqCount());

        n = m_urlTree.getUrlCount(urls[i]);
        EXPECT_EQ(1, n);

        m_urlTree.delUrl(urls[i]);
        n = m_urlTree.getUrlCount(urls[i]);
        EXPECT_EQ(0, n);
        EXPECT_EQ(0, m_urlTree.getAllUrlCount());
        EXPECT_EQ(0, m_urlTree.getAllFreqCount());
    }

    EXPECT_EQ(0, m_urlTree.getAllUrlCount());
    EXPECT_EQ(0, m_urlTree.getAllFreqCount());

    for (int i = 0; i < sizeof(urls) / sizeof(urls[0]); ++i)
    {
        int j = 0;
        for (j = 0; j < 10; j++)
        {
            n = m_urlTree.addUrl(urls[i]);
            EXPECT_EQ(j+1, n);
            EXPECT_EQ(1, m_urlTree.getAllUrlCount());
            EXPECT_EQ(j+1, m_urlTree.getAllFreqCount());
        }

        n = m_urlTree.getUrlCount(urls[i]);
        EXPECT_EQ(j, n);

        m_urlTree.delUrl(urls[i]);
        n = m_urlTree.getUrlCount(urls[i]);
        EXPECT_EQ(0, n);
        EXPECT_EQ(0, m_urlTree.getAllUrlCount());
        EXPECT_EQ(0, m_urlTree.getAllFreqCount());
    }

    EXPECT_EQ(0, m_urlTree.getAllUrlCount());
    EXPECT_EQ(0, m_urlTree.getAllFreqCount());

    const int loopCount = 10;
    for (int i = 0; i < sizeof(urls) / sizeof(urls[0]); ++i)
    {
        int j = 0;
        for (j = 0; j < loopCount; j++)
        {
            n = m_urlTree.addUrl(urls[i]);
            EXPECT_EQ(j + 1, n);
            EXPECT_EQ(i + 1, m_urlTree.getAllUrlCount());
            EXPECT_EQ(j + 1 + loopCount * i, m_urlTree.getAllFreqCount());
        }

        n = m_urlTree.getUrlCount(urls[i]);
        EXPECT_EQ(j, n);
    }

    n = m_urlTree.getUrlCount("http");
    EXPECT_EQ(0, n);

    n = m_urlTree.getUrlCount("https");
    EXPECT_EQ(0, n);

    n = m_urlTree.getUrlCount("file://");
    EXPECT_EQ(0, n);
}


TEST_F(CTrieTreeTester, delUrl)
{

}