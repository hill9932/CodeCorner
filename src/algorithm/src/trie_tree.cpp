#include "trie_tree.h"

namespace LabSpace
{
    namespace algorithm
    {
        CUrlTrieTree::CUrlTrieTree()
        {
            m_urlCount  = 0;
            m_freqCount = 0;
        }

        CUrlTrieTree::~CUrlTrieTree()
        {
            clear();
        }

        int CUrlTrieTree::addNode(TreeNode_t* _node, const tchar* _url)
        {
            if (!_url)  return -1;
            int childNum = *_url;
            TreeNode_t*& childNode = _node->childNodes[childNum];

            if (childNode == NULL)
                childNode = new TreeNode_t;

            childNode->nodeChar = *_url;

            tchar nextChar = *(_url + 1);
            if (!nextChar)  // the last character and increase the frequent number
            {
                if (childNode->freq == 0)
                    m_urlCount++;

                m_freqCount++;
                return ++childNode->freq;
            }
            else
                return addNode(childNode, _url + 1);
        }

        CUrlTrieTree::TreeNode_t* CUrlTrieTree::findNode(TreeNode_t* _node, const tchar* _url)
        {
            if (!_url)  return NULL;
            int childNum = *_url;
            TreeNode_t* childNode = _node->childNodes[childNum];
            if (childNode == NULL)
                return NULL;

            tchar nextChar = *(_url + 1);
            if (!nextChar && childNode->nodeChar == *_url)  // got it
                return childNode;

            return findNode(childNode, _url + 1);
        }

        int CUrlTrieTree::addUrl(const tstring& _url)
        {
            if (_url.empty())   return -1;
            return addUrl(_url.c_str());
        }    
        
        int CUrlTrieTree::addUrl(const tchar* _url)
        {
            if (!_url)  return -1;
            return addNode(&m_root, _url);
        }

        int CUrlTrieTree::getUrlCount(const tchar* _url)
        {
            if (!_url)   return -1;
            
            TreeNode_t* node = findNode(&m_root, _url);
            return node ? node->freq : 0;
        }

        int CUrlTrieTree::getUrlCount(const tstring& _url)
        {
            return getUrlCount(_url.c_str());
        }

        void CUrlTrieTree::delUrl(const tchar* _url)
        {
            if (!_url)   return;
            TreeNode_t* node = findNode(&m_root, _url);
            if (node)
            {
                m_freqCount -= node->freq;
                --m_urlCount;

                node->nodeChar = 0;
                node->freq = 0;
            }
        }

        void CUrlTrieTree::delUrl(const tstring& _url)
        {
            if (_url.empty())   return;
            return delUrl(_url.c_str());
        }

        void CUrlTrieTree::delNode(TreeNode_t* _node)
        {
            if (_node)
            {
                for (int i = 0; i < TreeNode_t::ChildCount; ++i)
                {
                    if (_node->childNodes[i])
                    {
                        delNode(_node->childNodes[i]);
                        delete _node->childNodes[i];
                        _node->childNodes[i] = NULL;
                    }
                }
            }
        }

        void CUrlTrieTree::clear()
        {
            delNode(&m_root);
        }
    }
}