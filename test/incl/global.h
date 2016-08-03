#ifndef __HL_GLOBAL_INCLUDE_H__
#define __HL_GLOBAL_INCLUDE_H__

#include <gtest/gtest.h>
#include <string>

struct config_t
{
    std::string configFile;
};
extern config_t g_conf;


/**
*@Function: this class is the global shared test environment.
**/
class MyEnvironment : public testing::Environment
{
public:
    MyEnvironment()
    {
        puts("Create MyEnvironment.");
    }

    virtual ~MyEnvironment()
    {
        puts("Destroy MyEnvironment.");
    }

    virtual void SetUp()
    {
        puts("Setup MyEnvironment.");
    }

    virtual void TearDown()
    {
        puts("Teardown MyEnvironment.");
    }
};


#endif
