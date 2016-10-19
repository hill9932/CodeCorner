#include <iostream>
#include <popt/popt.h>
#include "global.h"
#include "../src/log_.h"

config_t g_conf;
log4cplus::Logger*  g_logger;

int main(int _argc, char* _argv[])
{ 
#ifdef WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   // _CrtSetBreakAlloc(1400);
#endif

    char* configFileName = "";
    struct poptOption table[] =
    {
        { "config-file",
        '\0',
        POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
        &configFileName,
        1,
        "config file name.",
        "config.lua" }
    };

    //
    // parse the command line
    //
    poptContext context = poptGetContext(NULL, _argc, const_cast<const char**>(_argv), table, 0);
    int r = 0;
    while ((r = poptGetNextOpt(context)) >= 0);
    std::cerr << std::endl; //poptPrintHelp(context, stderr, 0); cerr << endl;
    if (r < -1)
    {
        poptPrintHelp(context, stderr, 0);
        std::cerr << "Has invalid command options.";
        poptFreeContext(context);
        return false;
    }
    poptFreeContext(context);

    g_conf.configFile = configFileName;

    //
    // start the test cases
    //
    testing::AddGlobalTestEnvironment(new MyEnvironment);
    testing::InitGoogleTest(&_argc, _argv);
    return RUN_ALL_TESTS();
}


/*
using boost test
#include <boost/test/included/unit_test.hpp>

using namespace boost::unit_test;

void test_case3()
{
}

test_suite*  init_unit_test_suite(int argc, char* argv[])
{
    framework::master_test_suite().add(BOOST_TEST_CASE(&test_case3));

    boost::shared_ptr<test_class> tester( new test_class );
    framework::master_test_suite().add( BOOST_TEST_CASE( boost::bind( &test_class::test_method1, tester )));

    boost::function<void (double)> test_method = bind( &test_class::test_method, &tester, _1);
    framework::master_test_suite().add( BOOST_PARAM_TEST_CASE( test_method, params, params+5 ) );

    test_suite* ts1 = BOOST_TEST_SUITE("test_suite1");
    test_suite* ts2 = BOOST_TEST_SUITE("test_suite2");
    ts2->add(BOOST_TEST_CASE(&test_case3));
    ts2->add(BOOST_TEST_CASE(&test_case4));

    framework::master_test_suite().add(ts1);
    framework::master_test_suite().add(ts2);

    return 0;
}
*/