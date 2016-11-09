#pragma once

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename DATA_T>
        struct ArrayNode
        {
            DATA_T data;
        };


        template<typename DATA_T, int SIZE>
        class CSortArray
        {
        public:
            typedef ArrayNode<DATA_T>   ArrayNode_t;
            typedef ArrayNode_t*        ArrayNodePtr;

            CSortArray();
            ~CSortArray();

        private:
            std::unique_ptr<ArrayNode_t[]>    m_nodes;
        };

        #include "sort_array.hxx"
    }
}
