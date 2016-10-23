#include "digest_task.h"
#include "../web_voyager.h"
#include <regex>
#include <fstream>


CFirstTask::CFirstTask()
{
    m_voyager       = NULL;
    m_taskManager   = NULL;
}

bool CFirstTask::init()
{
    m_voyager       = CWebVoyager::GetInstance();
    m_taskManager   = CTaskManager::GetInstance();
    return true;
}

void* CFirstTask::operator()(void* _item)
{
    while (!m_taskManager->isStop())
    {
        if (m_finishedReqPool.size())
        {
            HTTPClient_t* hc = m_finishedReqPool.back().get();
            m_finishedReqPool.pop_back();

            hc->status = HTTPClient_t::STATUS_PROCESSING;
            return hc;
        }
        else
        {
            m_voyager->swap(m_finishedReqPool);
            if (m_finishedReqPool.size() == 0)
                SleepMS(1);
        }
    }

    return NULL;
}



void CDigestTask::process(HTTPClient_t* _hc)
{
    if (!_hc)    return;

    struct evbuffer* evbuf = evhttp_request_get_input_buffer(_hc->request);
    const char* buf = (const char*)evbuffer_pullup(evbuf, -1);
    int bufSize = _hc->request->body_size;

    /*
    std::ofstream ofs;
    ofs.open("output.html", std::ofstream::binary);
    ofs.write(buf, bufSize);
    ofs.close();
    */

    const std::regex pattern("<a href=\"((?![#|javascript]).*?)\""); // skip the <a href="#...." and <a href="javascript:
    const std::cregex_token_iterator end;
    for (std::cregex_token_iterator it(buf, buf + bufSize, pattern, 1); it != end; ++it)
    {
        L4C_LOG_TRACE(it->str());
    }
}

void* CDigestTask::operator()(void* _item)
{
    process((HTTPClient_t*)_item);
    return _item;
}