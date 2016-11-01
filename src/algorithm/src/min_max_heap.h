#pragma once

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename KEY, typename DATA>
        struct HeapNode
        {
            KEY     key;
            DATA    data;
        };


        template<typename KEY, typename DATA, int SIZE>
        class CHeap
        {
        public:
            CHeap();
            ~CHeap();

            typedef HeapNode<KEY, DATA> NodeType;
            int addNode(const NodeType* _node);
            int addNode(const NodeType& _node);
            void createMaxHeap();
            void createMinHeap();
            void sortHeap();
            bool validate();

            NodeType* getNodes(){ return m_nodes.get(); }
            int getNodesCount() { return m_nodesCount; }

        private:
            typedef bool(*CompFunc)(const KEY&, const KEY&);

            void maxOrMinHeap(int _nodeLevel, CompFunc _func);
            void exchangeNodes(int _l, int _r);

        private:
            std::unique_ptr<NodeType[]>   m_nodes;
            int m_nodesCount;
        };

        #include "min_max_heap.hxx"
    }
}