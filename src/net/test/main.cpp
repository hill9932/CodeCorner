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

    poptContext context = poptGetContext(NULL, _argc, const_cast<const char**>(_argv), table, 0);

    int r = 0;
    while ((r = poptGetNextOpt(context)) >= 0);
    std::cerr << std::endl; 
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

