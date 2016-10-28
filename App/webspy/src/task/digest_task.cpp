#include "digest_task.h"
#include "../web_voyager.h"
#include <regex>
#include <fstream>


CGetWebPageTask::CGetWebPageTask()
{
}

void* CGetWebPageTask::operator()(void* _item)
{
    CWebVoyager* voyager    = CWebVoyager::GetInstance();
    CPipeline* pipeline     = CProcessPipeline::GetInstance();

    while (!pipeline->isStop())
    {
        if (m_pendingRecords.size())
        {
            HTTPClientRawPtr hc = m_pendingRecords.back();
            m_pendingRecords.pop_back();
            
            hc->status = HTTPClient_t::STATUS_PROCESSING;
            L4C_LOG_DEBUG("CGetWebPageTask issue# " << ++m_issueCount << " with id = " << hc->id);
           
            assert(hc);
            return hc;
        }
        else
        {
            voyager->getHCRecords(m_pendingRecords);
            if (m_pendingRecords.size() == 0)
                SleepMS(1);
            else
                L4C_LOG_INFO("CGetWebPageTask get " << m_pendingRecords.size() << " pages from internet.");
        }
    }

    return NULL;
}


void CDigestTask::process(HTTPClientRawPtr _hc)
{
    if (!_hc || _hc->respCode != HTTP_OK)    return;

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

            if (str[0] != '/')
                fullStr += "/";

            fullStr += str;
            v.push_back(fullStr);
        }
        else
            v.push_back(str);

        L4C_LOG_TRACE(str);
    }
    
    if (v.size())
    {
        L4C_LOG_TRACE("Get new urls from " << _hc->url);
        CWebVoyager*  voyager = CWebVoyager::GetInstance();
        voyager->addRecords(v);
    }
}

void* CDigestTask::operator()(void* _item)
{
    HTTPClientRawPtr hc = (HTTPClientRawPtr)_item;
    hc->status = HTTPClient_t::STATUS_PROCESSING;

    u_int64 issueCount = ++m_issueCount;
    L4C_LOG_DEBUG("CDigestTask issue# " << issueCount << " with id = " << hc->id);

    process(hc);    

    L4C_LOG_DEBUG("CDigestTask issue# " << issueCount << " with id = " << hc->id << " finished.");

    hc->status = HTTPClient_t::STATUS_FINISHED;
    CWebVoyager::GetInstance()->freeHC(hc);
    return _item;
}