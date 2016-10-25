#include "collect_task.h"
#include "../web_voyager.h"


CCollectURLTask::CCollectURLTask()
{
    m_curRecordIndex = 0;
}


void* CCollectURLTask::operator()(void* _item)
{
    CWebVoyager* voyager = CWebVoyager::GetInstance();
    CPipeline* pipeline  = CCollectPipeline::GetInstance();

    while (!pipeline->isStop())
    {
        if (m_curRecordIndex >= m_urlRecords.size())
        {
            m_curRecordIndex = 0;
            m_urlRecords.clear();
            voyager->getUrlRecords(m_urlRecords);
        }

        if (m_curRecordIndex < m_urlRecords.size())
            return m_urlRecords[m_curRecordIndex++];
        else
            SleepMS(1);
    }

    return 0;
}

void* CDownloadTask::operator()(void* _item)
{
    CWebVoyager* voyager = CWebVoyager::GetInstance();
    URL_RECORD_t* record = (URL_RECORD_t*)_item;
    voyager->loadPage(record);
    delete record;

    return 0;
}