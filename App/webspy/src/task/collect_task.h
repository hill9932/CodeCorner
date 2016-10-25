#ifndef __HL_COLLECT_TASK_INCLUDE_H__
#define __HL_COLLECT_TASK_INCLUDE_H__

#include "task.h"
#include "../structure.h"


class CCollectURLTask : public CSerialTask
{
public:
    CCollectURLTask();
    void* operator()(void* _item);

private:
    vector<URL_RECORD_t*>   m_urlRecords;
    int m_curRecordIndex;
};


class CDownloadTask : public CParallelTask
{
public:
    void* operator()(void* _item);
};

#endif
