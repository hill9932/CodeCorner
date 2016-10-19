#ifndef __HL_WEB_SERVER_INCLUDE_H__
#define __HL_WEB_SERVER_INCLUDE_H__

#include "basic_manager.h"

/**
 * @Function: This class start a simple http server
 **/
class CWebServer : public CBasicManager, public ISingleton<CWebServer>
{
public:
    CWebServer();
    ~CWebServer();

    bool start();
    void join();

private:
    virtual bool    __init__();

};

#endif
