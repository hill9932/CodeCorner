#ifndef __HL_HTTP_BASIC_INCLUDE_H__
#define __HL_HTTP_BASIC_INCLUDE_H__

#include "common.h"

#include <string>
#include <event2/event.h>  
#include <event2/buffer.h>  
#include <event2/http.h>  
#include <event2/http_struct.h>  
#include <event2/keyvalq_struct.h> 

using namespace std;


#define DECLARE_STRING_CONSTANT(name_, value_) \
namespace Private                   \
{                                   \
    template <typename T>           \
    struct name_                    \
    {                               \
        static char const Name[];   \
        static char const Value[];  \
    };                              \
\
    template <typename T>           \
    char const name_ <T>::Name[] = #name_;      \
\
    template <typename T>           \
    char const name_ <T>::Value[] = #value_;    \
}                                   \
typedef Private::name_<void> name_;


namespace Network
{
    bool InitNetWork();

    namespace Http
    {
        namespace Request
        {
            namespace Header
            {
                DECLARE_STRING_CONSTANT(Accept, Accept)
                DECLARE_STRING_CONSTANT(AcceptCharset, Accept-Charset)
                DECLARE_STRING_CONSTANT(Host, Host)
                DECLARE_STRING_CONSTANT(UserAgent, User-Agent)
                // ...
            }

            namespace Content
            {
                DECLARE_STRING_CONSTANT(ContentLength,  Content-Length)
                DECLARE_STRING_CONSTANT(ContentType,    Content-Type)
            }
        }

        namespace Response
        {
            namespace Header
            {
                DECLARE_STRING_CONSTANT(AccessControlAllowOrigin, Access-Control-Allow-Origin)
                DECLARE_STRING_CONSTANT(AcceptRanges, Accept-Ranges)
                DECLARE_STRING_CONSTANT(Location, Location)
                // ...
            }

            namespace Code
            {
                DECLARE_STRING_CONSTANT(INFO_100,       Continue)
                DECLARE_STRING_CONSTANT(SUCCESS_200,    OK)
                DECLARE_STRING_CONSTANT(REDIRECT_302,   Found)
                DECLARE_STRING_CONSTANT(CLIENT_404,     Not Found)
                DECLARE_STRING_CONSTANT(SERVER_500,     Internal Error)
            }
        }

        struct CHttpUtils
        {
            static void ShowRequestHeadInfo(const struct evkeyvalq* _header);
            static void ShowRequestInfo(struct evhttp_request* _request);
            static void ShowUrlInfo(const struct evhttp_uri* _uri);
        };
    }
}


#endif
