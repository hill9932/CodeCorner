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
            bool selectMaxN(int _k);
            bool selectMinN(int _k);

            NodeType* getNode(int _index) 
            { 
                if (_index >= m_nodesCount) return NULL;
                return &m_nodes[_index];
            }
            NodeType* getNodes(){ return m_nodes.get(); }
            int getNodesCount() { return m_nodesCount; }

        protected:
            typedef bool(*CompFunc)(const KEY&, const KEY&);

            void maxOrMinHeap(int _nodeLevel, CompFunc _func);
            void exchangeNodes(int _l, int _r);

        private:
            int  partition(int _left, int _right, CompFunc _func);
            int  choosePivotByRandom(int _left, int _right, int _k);
            int  choosePivotByMedian(int _left, int _right, int _k);
            int  choosePivotByDirect(int _left, int _right, int _k);
            bool selectMaxN(int _left, int _right, int _k);
            bool selectMinN(int _left, int _right, int _k);

        protected:
            std::unique_ptr<NodeType[]>   m_nodes;
            int m_nodesCount;
        };


        /**
         * @Function: use top min heap
         **/
        template<typename KEY, typename DATA, int SIZE>
        class CTopNHeap : public CHeap<KEY, DATA, SIZE>
        {
        public:
            bool checkNode(const NodeType* _node);
            bool checkNode(const NodeType& _node);

        private:
            bool validate();
        };

        #include "min_max_heap.hxx"
    }
}