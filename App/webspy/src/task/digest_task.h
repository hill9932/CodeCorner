#ifndef __HL_DIGEST_INCLUDE_H__
#define __HL_DIGEST_INCLUDE_H__

#include "task.h"
#include "../structure.h"


/**
* @Function: This task do nothing except decide when to stop
**/
class CWebVoyager;
class CTaskManager;
class CFirstTask : public CSerialTask
{
public:
    CFirstTask();
    void* operator()(void* _item);
    virtual bool init();

private:

private:
    CWebVoyager*    m_voyager;
    CTaskManager*   m_taskManager;

    vector<HTTPClientPtr>   m_finishedReqPool;
};


class CDigestTask : public CParallelTask
{
public:
    void* operator()(void* _item);

private:
    void process(HTTPClient_t* hc);

};

#endif
