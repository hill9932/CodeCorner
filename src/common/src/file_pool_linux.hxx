#ifdef LINUX

#define MAX_EVENTS  16
int CFilePool::getFinishedRequest(vector<PPER_FILEIO_INFO_t>& _request, int _timeout)
{
    struct io_event events[MAX_EVENTS] = { 0 };

    int num_events = 0;
    if (getAioObject() == 0)  return -1;

    struct timespec timeout = {_timeout / 1000, (_timeout % 1000) * 1000000};  
    num_events = io_getevents(getAioObject(), 1, MAX_EVENTS, events, &timeout);
    if (num_events < 0)
    {
        return 0;
    }

    for (int i = 0; i < num_events; i++)
    {
        struct io_event event = events[i];
        PPER_FILEIO_INFO_t req = static_cast<PPER_FILEIO_INFO_t>(event.obj);
        if (!req)   continue;

        req->flag = event.res;
        _request.push_back(req);
    }

    return _request.size();
}

int CFilePool::getFinishedRequest()
{
    struct io_event events[MAX_EVENTS] = { 0 };

    int num_events = 0;
    int count = 0;
    if (getAioObject() == 0)  return -1;

    struct timespec timeout = {0, 0};  
    num_events = io_getevents(getAioObject(), 1, MAX_EVENTS, events, &timeout);
    if (num_events < 0)
    {
        return 0;
    }

    for (int i = 0; i < num_events; i++)
    {
        struct io_event event = events[i];
        PPER_FILEIO_INFO_t req = static_cast<PPER_FILEIO_INFO_t>(event.obj);
        if (!req)   continue;
        
        req->flag = event.res;

        if (m_aioCallback)
        {
           // req->flag = GetLastSysError();
            m_aioCallback(m_context, req);
        }
        else if (FILE_READ == req->optType)
        {
            if (m_readCallback) m_readCallback(m_context, req, req->dataLen);
        }
        else if (FILE_WRITE == req->optType)
        {
            if (m_writeCallback)m_writeCallback(m_context, req, req->dataLen);
        }

        ++count;
    }

    return count;
}

int CFilePool::IoHandler::operator()()
{
    struct io_event events[MAX_EVENTS] = { 0 };
    if (m_filePool->getAioObject() == 0)  return -1;

    while (!m_filePool->m_isStop)
    {
        int num_events = 0;

        struct timespec timeout = {1, 0};  
        num_events = io_getevents(m_filePool->getAioObject(), 1, MAX_EVENTS, events, &timeout);
        if (num_events < 0)
        {
            if (-num_events != EINTR)
            {
                RM_LOG_ERROR(GetLastSysErrorMessage(-num_events));
                return -1;
            }

            YieldCurThread();

            continue;
        }

        for (int i = 0; i < num_events; i++)
        {
            struct io_event event = events[i];
            PPER_FILEIO_INFO_t req = static_cast<PPER_FILEIO_INFO_t>(event.obj);
            assert(req);
            if (!req)   continue;

            req->flag = event.res;

            if (FILE_WRITE == req->optType && m_filePool->m_writeCallback)
            {
                ++m_totalWrite;
                m_filePool->m_writeCallback(NULL, req, event.res);
            }
            else if (FILE_READ == req->optType && m_filePool->m_readCallback)
            {
                ++m_totalRead;
                m_filePool->m_readCallback(NULL, req, event.res);
            }
            else 
                assert(false);

            //m_filePool->release(req);    // this pool allocates request
        }

        m_total += num_events;
    }

    return -1;  // no longer execute this handler any more
}

#endif

