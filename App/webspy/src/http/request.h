#ifndef __HL_HTTP_REQUEST_INCLUDE_H
#define __HL_HTTP_REQUEST_INCLUDE_H

#include "basic.h"
#include <memory>
#include <unordered_map>


#define HTTP_CONTENT_TYPE_URL_ENCODED   "application/x-www-form-urlencoded"     // default
#define HTTP_CONTENT_TYPE_FORM_DATA     "multipart/form-data"                   // (use for files: picture, mp3, tar-file etc.) 
#define HTTP_CONTENT_TYPE_TEXT_PLAIN    "text/plain"                            // (use for plain text)  


namespace Network
{
    struct IHttpRequest
    {
        enum class Type
        {
            HEAD, GET, PUT, POST
        };

        typedef std::unordered_map<std::string, std::string> RequestParams;

        virtual ~IHttpRequest() {}
        virtual Type GetRequestType() const = 0;
        virtual std::string const GetHeaderAttr(char const *attrName) const = 0;
        virtual std::size_t GetContentSize() const = 0;
        virtual void GetContent(void *buf, std::size_t len, bool remove) const = 0;
        virtual std::string const GetPath() const = 0;
        virtual RequestParams const GetParams() const = 0;
        virtual void SetResponseAttr(std::string const &name, std::string const &val) = 0;
        virtual void SetResponseCode(int code) = 0;
        virtual void SetResponseString(std::string const &str) = 0;
        virtual void SetResponseBuf(void const *data, std::size_t bytes) = 0;
        virtual void SetResponseFile(std::string const &fileName) = 0;
    };

    typedef std::shared_ptr<IHttpRequest> IHttpRequestPtr;
}

#endif
