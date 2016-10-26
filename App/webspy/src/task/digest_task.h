#ifndef __HL_DIGEST_INCLUDE_H__
#define __HL_DIGEST_INCLUDE_H__

#include "task.h"
#include "../structure.h"


/**
* @Function: This task do nothing except decide when to stop
**/
class CGetWebPageTask : public CSerialTask
{
public:
    CGetWebPageTask();
    void* operator()(void* _item);

private:
    vector<HTTPClientRawPtr>   m_pendingRecords;
};


class CDigestTask : public CParallelTask
{
public:
    void* operator()(void* _item);

private:
    void process(HTTPClientRawPtr hc);

};

#endif
