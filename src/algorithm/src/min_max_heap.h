#pragma once

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename KEY_T, typename DATA_T>
        struct HeapNode
        {
            KEY_T     key;
            DATA_T    data;
        };


        template<typename KEY_T, typename DATA_T, int SIZE>
        class CHeap
        {
        public:
            typedef HeapNode<KEY_T, DATA_T> HeapNode_t;
            typedef HeapNode_t*             HeapNodePtr;

            CHeap();
            ~CHeap();

            int addNode(const HeapNode_t* _node);
            int addNode(const HeapNode_t& _node);
            void createMaxHeap();
            void createMinHeap();
            void sortHeap();
            bool validate();
            bool selectMaxN(int _k);
            bool selectMinN(int _k);

            HeapNodePtr getNode(int _index)
            { 
                if (_index >= m_nodesCount) return NULL;
                return &m_nodes[_index];
            }
            HeapNodePtr getNodes(){ return m_nodes.get(); }
            int getNodesCount() { return m_nodesCount; }

        protected:
            typedef bool(*CompFunc)(const KEY_T&, const KEY_T&);

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
            std::unique_ptr<HeapNode_t[]>   m_nodes;
            int m_nodesCount;
        };


        /**
         * @Function: use top min heap
         **/
        template<typename KEY_T, typename DATA_T, int SIZE>
        class CTopNHeap : public CHeap<KEY_T, DATA_T, SIZE>
        {
        public:
            bool checkNode(const HeapNode_t* _node);
            bool checkNode(const HeapNode_t& _node);

        private:
            bool validate();
        };

        #include "min_max_heap.hxx"
    }
}