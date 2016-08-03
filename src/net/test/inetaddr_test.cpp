#include "global.h"
#include "../src/inetaddr.h"

using namespace LabSpace::Common;
using namespace LabSpace::Net;

class CInetAddrTester : public testing::TestWithParam<const char*>
{
public:
    /**
    * Get and show all the local ip addresses including v4 and v6
    **/
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

        ShowLocalIPs();
    }

    static void TearDownTestCase()
    {
#ifdef WIN32
        WSACleanup();
#endif

        CInetAddr::freeLocalAddrs(m_v4Addr);
        CInetAddr::freeLocalAddrs(m_v6Addr);
    }
    
    static void ShowLocalIPs()
    {
        CInetAddr::getLocalAddrs(&m_v4Addr, AF_INET);
        CInetAddr::getLocalAddrs(&m_v6Addr, AF_INET6);

        EXPECT_TRUE(m_v4Addr);
        EXPECT_TRUE(m_v6Addr);

        if (m_v4Addr)
        {
            local_addr_t* addr = m_v4Addr;
            while (addr)
            {
                CInetAddr iaddr(addr->addr);
                cout << iaddr.ip() << endl;
                addr = addr->next;
            }
        }

        if (m_v6Addr)
        {
            local_addr_t* addr = m_v6Addr;
            while (addr)
            {
                CInetAddr iaddr(addr->addr);
                cout << iaddr.ip() << endl;
                addr = addr->next;
            }
        }
    }

    virtual void SetUp()
    {
        m_ipAddr = GetParam();
    }

    virtual void Teardown()
    {

    }

public:
    static local_addr_t*    m_v4Addr;
    static local_addr_t*    m_v6Addr;
    const char* m_ipAddr;
};

local_addr_t* CInetAddrTester::m_v4Addr;
local_addr_t* CInetAddrTester::m_v6Addr;


/**
* Test the function CInetAddr::isIPAddress
**/
TEST(CInetAddr, IsValidAddress)
{
    tstring ip = "192.168.1.1";
    EXPECT_EQ(4, CInetAddr::isIPAddress(ip.c_str()));

    ip = "0.0.0.1";
    EXPECT_EQ(4, CInetAddr::isIPAddress(ip.c_str()));

    ip = "2000::1";
    EXPECT_EQ(6, CInetAddr::isIPAddress(ip.c_str()));

    ip = "::1";
    EXPECT_EQ(6, CInetAddr::isIPAddress(ip.c_str()));

    ip = "0.0.0.0";
    EXPECT_EQ(4, CInetAddr::isIPAddress(ip.c_str()));

    ip = "1.2.3.a";
    EXPECT_EQ(-1, CInetAddr::isIPAddress(ip.c_str()));

    ip = "FE08:0::1";
    EXPECT_EQ(6, CInetAddr::isIPAddress(ip.c_str()));

    ip = "f3:12:33::1";
    EXPECT_EQ(6, CInetAddr::isIPAddress(ip.c_str()));

    ip = "f3::12:33::1";
    EXPECT_EQ(-1, CInetAddr::isIPAddress(ip.c_str()));
}


TEST_P(CInetAddrTester, MakeAddr)
{
    CInetAddr addr;
    vector<tstring> v;
    bool b = false;
    StrUtil::StrSplit(tstring(m_ipAddr), tstring(":"), v);

    int ver = CInetAddr::isIPAddress(v[0].c_str());
    if (ver > 0)
    {
        if (v.size() == 1)  // only ip address
        {
            EXPECT_EQ(0, addr.makeAddr(m_ipAddr, NULL, false));
            EXPECT_EQ(v[0], addr.ip());
            EXPECT_EQ(ver == 4 ? AF_INET : AF_INET6, addr.family());
            b = true;
        }
        else    // ip + port
        {
            if (CInetAddr::isValidPort(v[1]) > 0)
            {
                EXPECT_EQ(0, addr.makeAddr(m_ipAddr, NULL, false));
                EXPECT_EQ(v[0], addr.ip());
                EXPECT_EQ(atoi(v[1].c_str()), addr.port());
                EXPECT_EQ(ver == 4 ? AF_INET : AF_INET6, addr.family());
                b = true;
            }
            else
            {
                EXPECT_EQ(-2, addr.makeAddr(m_ipAddr, NULL, false));
            }
        }
    }
    else
    {
        EXPECT_EQ(-1, addr.makeAddr(m_ipAddr, NULL, false));
    }

    cerr << "Check value '" << m_ipAddr << "' should be " << ( b ? "[true]" : "[false]") << endl;
}


/**
 * Set the values to test CInetAddrTester
 **/
INSTANTIATE_TEST_CASE_P(CInetAddr, CInetAddrTester, 
    testing::Values( 
            "invalid address",
            "192.168.1.2", 
            "192.168.1.2:1235", 
            "192.33", 
            "1923.23.1.2", 
            "123.23.23.0",
            "0.0.0.0",
            "0.2.3.2:124568",
            "a.1.2.3",
            "fe90::1",
            "fe90::1:89",
            "ab:ef::1",
            "dh::0"));