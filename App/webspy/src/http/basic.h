#ifndef __HL_HTTP_BASIC_INCLUDE_H__
#define __HL_HTTP_BASIC_INCLUDE_H__

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
    namespace Http
    {
        namespace Request
        {
            namespace Header
            {
                DECLARE_STRING_CONSTANT(Accept, Accept)
                DECLARE_STRING_CONSTANT(AcceptCharset, Accept-Charset)
                // ...
            }
        }

        namespace Response
        {
            namespace Header
            {
                DECLARE_STRING_CONSTANT(AccessControlAllowOrigin, Access-Control-Allow-Origin)
                DECLARE_STRING_CONSTANT(AcceptRanges, Accept-Ranges)
                // ...
            }
        }

        struct CHttpUtils
        {
            static void ShowRequestHeadInfo(struct evkeyvalq* _header);
        };
    }
}


#endif
