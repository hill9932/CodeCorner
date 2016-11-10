#pragma once

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename KEY_T>
        struct ArrayNode
        {
            KEY_T key;
        };


        template<typename KEY_T, int SIZE>
        class CSortArray
        {
        public:
            typedef ArrayNode<KEY_T>    ArrayNode_t;
            typedef ArrayNode_t*        ArrayNodePtr;

            CSortArray();
            ~CSortArray();

            /**
             * @Function: add value
             * @Return: 0 success
             *         -1 failed or is full
             **/
            int addValue(const KEY_T& _value);
            int getNodesCount() { return m_nodesCount; }

            /**
             * @Function: get value
             * @Return: 
             *  exception when _index is out of count
             **/
            KEY_T operator[](int _index);

            void sort();
            void validate();
            int  end()
            {
                if (m_nodesCount == 0)  return npos;
                return m_nodesCount - 1;
            }

            /**
             * @Function: shift the array, when _shiftNum > 0 shift right otherwise shift left
             **/
            void shift(int _shiftNum);
            void cycle(int _shiftNum);
            void reverse();

        protected:
            void validate(ArrayNodePtr _node);
            void shift(ArrayNodePtr _node, int _begin, int _end, int _shiftNum);
            void exchange(int _l, int _r);
            void reverse(int _begin, int _end);

        public:
            static const int npos = -1;

        private:
            std::unique_ptr<ArrayNode_t[]>    m_nodes;
            int     m_nodesCount;
        };

        #include "sort_array.hxx"
    }
}
