#ifndef __HL_BSEARCH_INCLUDE_H__
#define __HL_BSEARCH_INCLUDE_H__

#include "log_.h"

namespace LabSpace
{
    namespace algorithm
    {
        template<typename KEY_T, typename DATA_T>
        struct BSTreeNode
        {
            enum NodeColor { RED, BLACK };

            BSTreeNode()
            {
                color   = RED;
                level   = 0;
                freq    = 1;
                left   = right = parent = NULL;
                lHeight = 0;
                rHeight = 0;
            }

            int getChildsHeight()
            {
                return max(lHeight, rHeight);
            }

            KEY_T           key;
            DATA_T          data;
            NodeColor       color;

            int             freq;   // how many times added
            int             level;  // the distance to root
            int             lHeight;
            int             rHeight;
            BSTreeNode*     left;
            BSTreeNode*     right;
            BSTreeNode*     parent;
        };


        template<typename KEY_T, typename DATA_T>
        class CBSTree
        {
        public:
            typedef BSTreeNode<KEY_T, DATA_T>   TreeNode_t;
            typedef TreeNode_t*                 TreeNodePtr;

            CBSTree();
            ~CBSTree();

            /**
             * @Function: add value to tree
             * @Return: the freq of this value
             **/
            int  addValue(const KEY_T& _value);

            /**
             * @Function: decrement one of the freq
             * @Return: -1  no value found
             *          >0  current freq
             **/
            int  delValue(const KEY_T& _value);

            /**
             * @Function: remove the value node
             * @Return: -1  no value found
             *          >0  current freq
             **/
            int  removeValue(const KEY_T& _value);

            TreeNodePtr findNode(const KEY_T& _value);

            void clear();
            bool validate();
            int  getNodeCount() { return m_nodesCount; }
            int  getFreqCount() { return m_freqCount; }

        protected:
            void delNode(TreeNodePtr _node);
            void midOrder(TreeNodePtr _node, KEY_T* _buf, int& _n);
            TreeNodePtr grandparent(TreeNodePtr _node);
            TreeNodePtr uncle(TreeNodePtr _node);
            TreeNodePtr sibling(TreeNodePtr _node);

            virtual void adjust(TreeNodePtr _node);

        protected:
            TreeNodePtr     m_root;
            int             m_nodesCount;
            int             m_freqCount;
        };

        #include "bsearch_tree.hxx"
    }
}

#endif
