#include <gtest/gtest.h>
#include <numeric>
#include <algorithm>
#include <atomic>

#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "../src/common.h"
#include "../src/time_.h"
#include "../src/hash_.h"

using namespace LabSpace::Common;
using namespace tbb;

#define TEST_CASE_NAME  Test_hash_

std::atomic_uint32_t g_total = 0;

class ApplyFoo 
{
    vector<u_int32>& m_vec;

public:
    void operator()(const blocked_range<size_t>& r) const 
    {
        for (size_t i = r.begin(); i != r.end(); ++i)
        {
            u_int32 hash = HashIp(i);
            INTERLOCKED_INCREMENT(&m_vec[hash]);
        }
        g_total += r.size();
        cout << g_total << "        \r";
    }

    ApplyFoo(vector<u_int32>& _v) : m_vec(_v)
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

    parallel_for(blocked_range<size_t>(MIN_VALUE, MAX_VALUE, 1 << 20), ApplyFoo(hashIpHits));
    
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

