#include "global.h"
#include "inetaddr.h"
#include "dns_manager.h"
#include "web_voyager.h"

#include <popt/popt.h>

using namespace LabSpace::Net;

/**
 * @Function: Set the default settings
 **/
void InitConfig()
{
    g_AppConfig.configFile  = FileUtil::GetAppDir() + "config/webspy.lua";
    g_AppConfig.logFile     = FileUtil::GetAppDir() + "config/web_spy_log4cplus.properties";
}


/**
 * @Function: Load the settings from config file
 **/
bool LoadConfig(int _argc, char* _argv[])
{
    char* configFileName = NULL;

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

    return true;
}


int main(int _argc, char* _argv[])
{
    if (!LoadConfig(_argc, _argv))
    {
        PRINT_MSG("Fail to load the app config.");
        return -1;
    }

    if (!InitLog(g_AppConfig.logFile, "WebSpy"))
        return -1;

    if (0 != CInetAddr::init())
    {
        L4C_LOG_ERROR("Fail to init inet.");
        return -1;
    }


//     CDNSManager* dnsManager = CDNSManager::GetInstance();
//     dnsManager->addWebName("www.sina.com.cn");
//     dnsManager->start();

    CWebVoyager* webVoyager = CWebVoyager::GetInstance();
    webVoyager->start();

//     dnsManager->join();
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