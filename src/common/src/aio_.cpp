#include "aio_.h"


#ifdef WIN32

CAsyncIO::CAsyncIO()
{
    m_aioHandler = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    ON_ERROR_PRINT_LASTMSG_AND_DO(m_aioHandler, == , NULL, throw "failed to create IOCP");
}

CAsyncIO::~CAsyncIO()
{
    if (m_aioHandler)   CloseHandle(m_aioHandler);
}

Aio_t CAsyncIO::assoicateObject(handle_t _fd, void* _context, u_int32 _flag)
{
    if (_fd == INVALID_HANDLE_VALUE || _fd == NULL)   return NULL;
    if (m_aioHandler == INVALID_HANDLE_VALUE || m_aioHandler == NULL)   return NULL;

    return CreateIoCompletionPort(_fd, m_aioHandler, (ULONG_PTR)_context, _flag);
}

#else

#define NUM_EVENTS  128

CAsyncIO::CAsyncIO()
{
    //m_aioHandler.create();
    m_aioHandler = NULL;
    int z = io_setup(NUM_EVENTS, &m_aioHandler);
    ON_ERROR_PRINT_LASTMSG_AND_DO(z, != , 0, throw "Fail to step aioContext");
}

CAsyncIO::~CAsyncIO()
{
    io_destroy(m_aioHandler);
    m_aioHandler = 0;
}

Aio_t CAsyncIO::assoicateObject(handle_t _fd, void* _context, u_int32 _events)
{
    //m_aioHandler.registerEvent(_fd, _events, NULL, _context);
    return m_aioHandler;
}

#endif
