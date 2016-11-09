#pragma once

#include "log_.h"


namespace LabSpace
{
    namespace algorithm
    {
        template<typename DATA>
        struct HashNode
        {
            DATA        data;
            int         count;
            HashNode<DATA>*   next;
        };

        template<typename DATA>
        class CHashMap
        {
            typedef HashNode<DATA>  HashNode_t;
            typedef HashNode_t*     HashNodePtr;

        public:
            CHashMap();
            ~CHashMap();

            /**
             * @Function: add node 
             * @Return: the node count.
             **/
            int addNode(const DATA& _data);

            /**
             * @Function: decrement node count when drop to 0 remove the node
             * @Return: the node count.
             *          -1  not exist
             **/
            int delNode(const DATA& _data);

            /**
             * @Function: remove a node
             * @Return: 0  success
             *         -1  not exist
             **/
            int removeNode(const DATA& _data);

            int getNodesCount() { return m_nodesCount; }

            HashNodePtr findNode(const DATA& _data);

        private:
            #define HASHLEN 2807303 

            std::unique_ptr<HashNodePtr[]>  m_nodes;
            int m_nodesCount;
        };

        #include "hash_map.hxx"
    }
}
