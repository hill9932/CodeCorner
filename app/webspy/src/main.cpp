#include "global.h"
#include "dns_manager.h"
#include "web_voyager.h"

#include <popt/popt.h>


/**
 * @Function: Set the default settings
 **/
void InitConfig()
{
    g_AppConfig.configFile  = FileUtil::GetAppDir() + "config/webspy.lua";
    g_AppConfig.logFile     = FileUtil::GetAppDir() + "config/webspy_log4cplus.properties";
    g_AppConfig.dbPath      = FileUtil::GetAppDir() + "db";

    FileUtil::CreateAllDir(g_AppConfig.dbPath);
}


/**
 * @Function: Load the settings from config file
 **/
bool LoadConfig(int _argc, char* _argv[])
{
    InitConfig();

    char* configFileName = NULL;

    //
    // load the settings from command lines
    //
    struct poptOption table[] =
    {
        { "config-file",
            '\0',
            POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
            &configFileName,
            1,
            "The config file path",
            "/etc/webspy/config/setting.lua" 
        }
    };

    int r = -1;
    poptContext context = poptGetContext(NULL, _argc, const_cast<const char**>(_argv), table, 0);
    while ((r = poptGetNextOpt(context)) >= 0);
    cerr << endl; 
    if (r < -1)
    {
        poptPrintHelp(context, stderr, 0);
        cerr << "Has invalid command options.";
        poptFreeContext(context);
        return false;
    }
    poptFreeContext(context);

    if (configFileName) g_AppConfig.configFile = configFileName;

    //
    // load the settings from config file
    //

    return true;
}


int main(int _argc, char* _argv[])
{
#ifdef WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    if (!LoadConfig(_argc, _argv))
    {
        PRINT_MSG("Fail to load the app config.");
        return -1;
    }

    if (!InitLog(g_AppConfig.logFile, "WebSpy"))
    {
        PRINT_MSG("Fail to init log at " << g_AppConfig.logFile);
        return -1;
    }

    if (!Network::InitNetWork())
    {
        L4C_LOG_ERROR("Fail to init the newtork.");
        return -1;
    }

    L4C_LOG_INFO("Welcome to use webspy.");
    L4C_LOG_INFO("\tUsing libevent " << event_get_version());

    CWebVoyager* webVoyager = CWebVoyager::GetInstance();
    webVoyager->start();


    webVoyager->join();
    return 0;
}


//
// comment out codes
//
/**
 * @Function: use my own logger
    string logPath = FileUtil::GetAppDir();
    logPath += "/log";
    FileUtil::CreateAllDir(logPath);

    CFileLogger::initStatic();
    CFileLogger m_logger;
    m_logger.setLogPath(logPath);
    m_logger.start();
*/


/**
 * @Function: use dns lookup
     CDNSManager* dnsManager = CDNSManager::GetInstance();
     dnsManager->addWebName("www.sina.com.cn");
     dnsManager->start();
     dnsManager->join();
 */