#ifndef __HL_BSEARCH_INCLUDE_H__
#define __HL_BSEARCH_INCLUDE_H__

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename T>
        struct BSTreeNode
        {
            enum BFStatus
            {
                STATUS_RH,     // right tree is higher
                STATUS_EH,     // the same height
                STATUS_LH      // left tree is higher
            };

            BSTreeNode()
            {
                level   = 0;
                freq    = 1;
                lNode   = rNode = pNode = NULL;
                lHeight = 0;
                rHeight = 0;
            }

            int getChildsHeight()
            {
                return max(lHeight, rHeight);
            }

            T               data;
            int             freq;   // how many times added
            int             level;  // the distance to root
            int             lHeight;
            int             rHeight;
            BSTreeNode*     lNode;
            BSTreeNode*     rNode;
            BSTreeNode*     pNode;
        };


        template<class T>
        class CBSTree
        {
        public:
            CBSTree();
            ~CBSTree();

            /**
             * @Function: add value to tree
             * @Return: the freq of this value
             **/
            int  addValue(const T& _value);

            /**
             * @Function: decrement one of the freq
             * @Return: -1  no value found
             *          >0  current freq
             **/
            int  delValue(const T& _value);

            /**
             * @Function: remove the value node
             * @Return: -1  no value found
             *          >0  current freq
             **/
            int  removeValue(const T& _value);

            BSTreeNode<T>* findNode(const T& _value);

            void clear();
            bool validate();
            int  getNodeCount() { return m_nodesCount; }
            int  getFreqCount() { return m_freqCount; }

        private:
            void delNode(BSTreeNode<T>* _node);
            void adjustHeight(BSTreeNode<T>* _node);
            void midOrder(BSTreeNode<T>* _node, T* _buf, int& _n);

        private:
            BSTreeNode<T>*      m_root;
            int                 m_nodesCount;
            int                 m_freqCount;
        };

        #include "bsearch_tree.hxx"
    }
}

#endif
