#include "digest_task.h"
#include "../web_voyager.h"
#include <regex>
#include <fstream>


CFirstTask::CFirstTask()
{
}

bool CFirstTask::init()
{
    return true;
}

void* CFirstTask::operator()(void* _item)
{
    CWebVoyager*  voyager       = CWebVoyager::GetInstance();
    CTaskManager* taskManager   = CTaskManager::GetInstance();

    while (!taskManager->isStop())
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
            voyager->swap(m_finishedReqPool);
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

    vector<tstring> v;
    const std::regex pattern("<a href=\"((?![#|javascript]).*?)\""); // skip the <a href="#...." and <a href="javascript:
    const std::cregex_token_iterator end;
    for (std::cregex_token_iterator it(buf, buf + bufSize, pattern, 1); it != end; ++it)
    {
        tstring& str = it->str();
        if (str.size() < 5 ||
            0 != stricmp(str.substr(0, 4).c_str(), "http"))
        {
            const char* scheme = evhttp_uri_get_scheme(_hc->uri);
            const char* host = evhttp_uri_get_host(_hc->uri);
            if (!scheme || !host)   continue;

            tstring fullStr;
            fullStr =  scheme;
            fullStr += "://";
            fullStr += host;
            fullStr += str;
            v.push_back(fullStr);
        }
        else
            v.push_back(str);

        L4C_LOG_TRACE(str);
    }
    
    CWebVoyager*  voyager = CWebVoyager::GetInstance();
    voyager->addRecords(v);
}

void* CDigestTask::operator()(void* _item)
{
    process((HTTPClient_t*)_item);
    return _item;
}