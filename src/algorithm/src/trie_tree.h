#ifndef __HL_TRIE_TREE_INCLUDE_H__
#define __HL_TRIE_TREE_INCLUDE_H__

#include "log_.h"


namespace LabSpace
{
    namespace algorithm
    {
        template<int N>
        struct CTrieTreeNode
        {
            CTrieTreeNode()
            {
                freq     = 0;
                nodeChar = 0;
                bzero(childNodes, sizeof(childNodes));
            }

            int     freq;
            tchar   nodeChar;

            CTrieTreeNode*   childNodes[N];

            static const int ChildCount = N;
        };


        class CUrlTrieTree
        {
        public:
            CUrlTrieTree();
            ~CUrlTrieTree();

            /**
             * @Function: add the url in the tree
             * @Return: the time that this url has been added
             **/
            int addUrl(const tstring& _url);
            int addUrl(const tchar* _url);

            /**
             * @Function: delete the url in the tree
             * @Return: the count will be 0 after been deleted
             **/
            void delUrl(const tstring& _url);
            void delUrl(const tchar* _url);

            /**
             * @Function: get the frequent count of this url
             **/
            int getUrlCount(const tstring& _url);
            int getUrlCount(const tchar* _url);

            /**
             * @Function: get the number of ulrs have been added
             **/
            int getAllUrlCount()    { return m_urlCount;    }
            int getAllFreqCount()   { return m_freqCount;   }

            void clear();

        private:
            typedef CTrieTreeNode<256> TreeNode_t;

            int  addNode(TreeNode_t* _node, const tchar* _url);
            void delNode(TreeNode_t* _node);
            TreeNode_t* findNode(TreeNode_t* _node, const tchar* _url);

        private:
            TreeNode_t  m_root;
            int         m_urlCount;
            int         m_freqCount;
        };
    }
}

#endif
