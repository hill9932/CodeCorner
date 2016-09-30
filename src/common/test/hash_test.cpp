#include <gtest/gtest.h>
#include <numeric>
#include <algorithm>
#include <atomic>
#include <map>

#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"

#include "../src/common.h"
#include "../src/time_.h"
#include "../src/hash_.h"

using namespace LabSpace::Common;
using namespace tbb;

#define TEST_CASE_NAME  Test_hash_


TEST(TEST_CASE_NAME, CheckTwoIPs)
{
    const char* ip1 = "10.157.40.45";
    const char* ip2 = "10.157.136.10";
    struct in_addr addr4;

    inet_pton(AF_INET, ip1, &addr4);
    u_int16 ip1Hash = HashIp(htonl(addr4.s_addr));

    inet_pton(AF_INET, ip2, &addr4);
    u_int16 ip2Hash = HashIp(htonl(addr4.s_addr));

    u_int32 srcIP = 178096138;
    u_int32 dstIP = 178071597;
    u_int16 srcPort = 23;
    u_int16 dstPort = 1624;

    char buf[128] = { 0 };
    u_int32 value = ntohl(srcIP);
    inet_ntop(AF_INET, &value, buf, sizeof(buf));
    cout << "Source address = " << buf << ":" << srcPort << endl;

    bzero(buf, sizeof(buf));
    value = ntohl(dstIP);
    inet_ntop(AF_INET, &value, buf, sizeof(buf));
    cout << "Dest address = " << buf << ":" << dstPort << endl;

    u_int16 checkIp1 = HashIp(srcIP);
    u_int16 checkIp2 = HashIp(dstIP);
    u_int16 checkIpPort1 = HashIpPort(srcIP, srcPort);
    u_int16 checkIpPort2 = HashIpPort(dstIP, dstPort);
    u_int16 checkFlow = HashFlow(srcIP, srcPort, dstIP, dstPort);

}

class CTaskCheckHashIp
{
public:
    void operator()(const blocked_range<size_t>& r)
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            u_int16 hash = HashIp(i);
            if (hash == m_checkValue)
                m_r.insert(i);
        }
        m_count += r.size();
    }

    CTaskCheckHashIp(CTaskCheckHashIp& x, split)
    {
        m_count = 0;
    }

    CTaskCheckHashIp(u_int16 _value)
    {
        m_checkValue = _value;
        m_count = 0;
    }

    void join(const CTaskCheckHashIp& y)
    {
        m_r.insert(y.m_r.begin(), y.m_r.end());
        m_count += y.m_count;
    }

public:
    set<u_int32>    m_r;
    u_int16 m_checkValue;
    u_int32 m_count;
};

TEST(TEST_CASE_NAME, CheckHashIP)
{
    const u_int32 MIN_VALUE = 0;
    const u_int32 MAX_VALUE = 0xFFFFFFFF;
    const char* ip = "10.157.40.45";
    struct in_addr addr4;

    inet_pton(AF_INET, ip, &addr4);
    u_int16 hash = HashIp(addr4.s_addr);

    CTaskCheckHashIp sf(hash);
    parallel_reduce(blocked_range<size_t>(MIN_VALUE, MAX_VALUE, 1 << 16), sf);

    EXPECT_EQ(sf.m_count, u_int32(1 << 32) - 1);

    cout << "Total count = " << sf.m_count << endl;
    cout << "Conflict count = " << sf.m_r.size() << endl;

    set<u_int32>::const_iterator it = sf.m_r.begin();
    for (; it != sf.m_r.end(); ++it)
    {
        char buf[128] = { 0 };
        u_int32 value = *it;
        value = ntohl(value);

        inet_ntop(AF_INET, &value, buf, sizeof(buf));
       // cout << buf << endl;
    }
}


std::atomic_uint32_t g_total = 0;

class CTaskHashIp 
{
    vector<u_int32>& m_vec;

public:
    void operator()(const blocked_range<size_t>& r) const 
    {
        size_t s = r.begin();
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            u_int32 hash = HashIp(i);
            INTERLOCKED_INCREMENT(&m_vec[hash]);
        }
        g_total += r.size();
        cout << g_total << "        \r";
    }

    CTaskHashIp(vector<u_int32>& _v) : m_vec(_v)
    {}
};


TEST(TEST_CASE_NAME, HashIP)
{
    TimeUtil::CScopeTimer timer("HashIP");
    task_scheduler_init init;

    const u_int32 MIN_VALUE = 0;
    const u_int32 MAX_VALUE = 0xFFFFFFFF;
    vector<u_int32> hashIpHits;
    vector<u_int32> hashIPortHits;
    vector<u_int32> hashFlowHits;
    hashIpHits.resize(1 << 16);
    hashIPortHits.resize(1 << 16);

    parallel_for(blocked_range<size_t>(MIN_VALUE, MAX_VALUE, 1 << 20), CTaskHashIp(hashIpHits));
    
    EXPECT_EQ(g_total, u_int32(1 << 32) - 1);

    std::sort(hashIpHits.begin(), hashIpHits.end());
    cout << endl;
    cout << "Total =\t" << g_total << endl;
    cout << "Min =\t"   << hashIpHits[0] << endl;
    cout << "Max =\t"   << hashIpHits[hashIpHits.size() - 1] << endl;
    cout << "Avg =\t"   << g_total / (1 << 16) << endl;

    double sum = std::accumulate(std::begin(hashIpHits), std::end(hashIpHits), 0.0);
    double mean = sum / hashIpHits.size();
    EXPECT_EQ(sum, u_int32(1 << 32) -1);

    double accum = 0.0;
    std::for_each(std::begin(hashIpHits), std::end(hashIpHits), [&](const double d)
    {
        accum += (d - mean)*(d - mean);
    });

    double stdev = sqrt(accum / (hashIpHits.size() - 1)); // 
    cout << endl << "stdev = " << stdev << endl;
}


/*
TEST(TEST_CASE_NAME, HashIP_NO)
{
    TimeUtil::CScopeTimer timer("HashIP_NO");
    task_scheduler_init init;

    const u_int32 MIN_VALUE = 0;
    const u_int32 MAX_VALUE = 0xFFFFFFFF;
    vector<u_int32> hashIpHits;
    vector<u_int32> hashIPortHits;
    vector<u_int32> hashFlowHits;
    hashIpHits.resize(1 << 16);
    hashIPortHits.resize(1 << 16);

    for (u_int32 i = MIN_VALUE; i < MAX_VALUE; ++i)
    {
        u_int32 hash = HashIp(i);
        ++hashIpHits[hash];
        //INTERLOCKED_INCREMENT(&hashIpHits[hash]);

        //if (i % 10000000 == 0)
        //    cout << i << "\r";
    }

    std::sort(hashIpHits.begin(), hashIpHits.end());
    cout << endl;
    cout << "Total =\t" << g_total << endl;
    cout << "Min =\t" << hashIpHits[0] << endl;
    cout << "Max =\t" << hashIpHits[hashIpHits.size() - 1] << endl;
    cout << "Avg =\t" << g_total / (1 << 16) << endl;

    double sum = std::accumulate(std::begin(hashIpHits), std::end(hashIpHits), 0.0);
    double mean = sum / hashIpHits.size();
    EXPECT_EQ(sum, u_int32(1 << 32) - 1);

    double accum = 0.0;
    std::for_each(std::begin(hashIpHits), std::end(hashIpHits), [&](const double d)
    {
        accum += (d - mean)*(d - mean);
    });

    double stdev = sqrt(accum / (hashIpHits.size() - 1)); // 
    cout << endl << "stdev = " << stdev << endl;
    exit(0);
}*/