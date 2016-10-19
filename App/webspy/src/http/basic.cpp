#include "basic.h"

namespace Network
{
    namespace Http
    {
        void CHttpUtils::ShowRequestHeadInfo(struct evkeyvalq* _header)
        {
            if (!_header)   return;

            struct evkeyval *first_node = _header->tqh_first;
            while (first_node) {
              //  cerr << "key:%s  value:%s", first_node->key, first_node->value;
                first_node = first_node->next.tqe_next;
            }
        }
    }
}