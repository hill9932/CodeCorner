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
        if (m_curRecordIndex >= m_urlRecords.size() &&
            voyager->getPendingRequest() < PROCESS_RECORDS_BATCH_COUNT)
        {
            m_curRecordIndex = 0;
            m_urlRecords.clear();
            voyager->getUrlRecords(m_urlRecords);

            if (m_urlRecords.size())
                L4C_LOG_INFO("CCollectURLTask get " << m_urlRecords.size() << " urls from database.");
        }

        if (m_curRecordIndex < m_urlRecords.size())
        {
            L4C_LOG_DEBUG("CCollectURLTask issue# " << ++m_issueCount << " with id = " << m_urlRecords[m_curRecordIndex]->id);
            URL_RECORD_t* record = m_urlRecords[m_curRecordIndex++];
            assert(record);
            return record;
        }
        else
            SleepMS(1);
    }

    return 0;
}

void* CDownloadTask::operator()(void* _item)
{
    CWebVoyager* voyager = CWebVoyager::GetInstance();
    URL_RECORD_t* record = (URL_RECORD_t*)_item;

    u_int64 issueCount = ++m_issueCount;

    L4C_LOG_DEBUG("CDownloadTask issue# " << issueCount << " with id = " << record->id);
    voyager->loadPage(record);
    L4C_LOG_DEBUG("CDownloadTask issue# " << issueCount << " with id = " << record->id << " finished.");

    delete record;

    return 0;
}