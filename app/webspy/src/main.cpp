#include <event.h>  
#include <event2/util.h>  

#include "file_util.h"
#include "file_log.h"
#include "inetaddr.h"
#include "dns_manager.h"

using namespace LabSpace::Net;

int main(int _argc, char* _argv[])
{
    if (0 != CInetAddr::init())
    {
        LOG_ERROR_STREAM << "Fail to init inet.";
        return -1;
    }

    string logPath = FileUtil::GetAppDir();
    logPath += "/log";
    FileUtil::CreateAllDir(logPath);

    CFileLogger::initStatic();
    CFileLogger m_logger;
    m_logger.setLogPath(logPath);
//    m_logger.start();

    CDNSManager* manager = CDNSManager::GetInstance();
    manager->addWebName("www.sina.com.cn");
    manager->start();
    manager->join();

    return 0;
}