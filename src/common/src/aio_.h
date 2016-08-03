#ifndef __HL_AIO_INCLUDE_H__
#define __HL_AIO_INCLUDE_H__

#include "log_.h"

#ifdef LINUX
    #include "poll.h"
#endif

class CAsyncIO
{
public:
    CAsyncIO();
    ~CAsyncIO();

    operator Aio_t() { return m_aioHandler; }

    /**
     * @Function: associate the handle with IOCP
     * @Param _flag: in windows, it specify the The maximum number of threads that
     *               the operating system can allow to concurrently process.
     *               in linux, it specifies the interested event type
     * @Return: NULL failed
     **/
    Aio_t   assoicateObject(handle_t _fd, void* _context = NULL, u_int32 _flag = 0);

    Aio_t   m_aioHandler;
};

#endif
