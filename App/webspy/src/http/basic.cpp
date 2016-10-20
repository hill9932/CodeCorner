#include "basic.h"
#include "log_.h"


namespace Network
{
    namespace Http
    {
        void CHttpUtils::ShowRequestHeadInfo(const struct evkeyvalq* _header)
        {
            if (!_header)   return;

            L4C_LOG_TRACE("Heads Information:");
            struct evkeyval *first_node = _header->tqh_first;
            while (first_node)
            {
                L4C_LOG_TRACE("\t" << first_node->key << " = " << first_node->value);
                first_node = first_node->next.tqe_next;
            }
        }

        void CHttpUtils::ShowRequestInfo(struct evhttp_request* _request)
        {
            if (!_request)  return;

            ShowRequestHeadInfo(_request->output_headers);

            /*
            struct evbuffer* buf = evhttp_request_get_input_buffer(_request);
            L4C_LOG_TRACE("Body size = " << _request->body_size);
            L4C_LOG_TRACE("HTML Body = " << evbuffer_pullup(buf, -1));
            */
        }

        void CHttpUtils::ShowUrlInfo(const struct evhttp_uri* _uri)
        {
            if (!_uri)  return;

            const char* scheme  = evhttp_uri_get_scheme(_uri);
            const char* host    = evhttp_uri_get_host(_uri);
            const char* path    = evhttp_uri_get_path(_uri);
            const char* query   = evhttp_uri_get_query(_uri);
            const char* userinfo = evhttp_uri_get_userinfo(_uri);
            const char* fragment = evhttp_uri_get_fragment(_uri);

            L4C_LOG_TRACE("scheme = "   << (scheme ? scheme : ""));
            L4C_LOG_TRACE("host = "     << (host ? host : ""));
            L4C_LOG_TRACE("path = "     << (path ? path : ""));
            L4C_LOG_TRACE("port = "     << evhttp_uri_get_port(_uri));
            L4C_LOG_TRACE("query = "    << (query ? query : ""));
            L4C_LOG_TRACE("userinfo = " << (userinfo ? userinfo : ""));
            L4C_LOG_TRACE("fragment = " << (fragment ? fragment : ""));
        }

    }
}