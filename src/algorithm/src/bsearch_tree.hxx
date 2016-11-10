
template<typename KEY_T, typename DATA_T>
CBSTree<KEY_T, DATA_T>::CBSTree()
{
    m_root = NULL;
    m_nodesCount = 0;
    m_freqCount = 0;
}

template<typename KEY_T, typename DATA_T>
CBSTree<KEY_T, DATA_T>::~CBSTree()
{
    clear();
}

template<typename KEY_T, typename DATA_T>
void CBSTree<KEY_T, DATA_T>::adjust(TreeNodePtr _node)
{
    if (!_node) return;

    while (_node)
    {
        TreeNodePtr parent = _node->parent;
        if (parent)
        {
            int childHeight = parent->getChildsHeight();

            if (parent->left == _node)
                parent->lHeight++;
            else
                parent->rHeight++;

            if (childHeight >= parent->getChildsHeight()) // no need notify above
                break;
        }

        _node = parent;
    }
}

template<typename KEY_T, typename DATA_T>
int CBSTree<KEY_T, DATA_T>::addValue(const KEY_T& _value)
{
    if (!m_root)
    {
        TreeNodePtr newNode = new TreeNode_t;
        newNode->key = _value;
        ++m_nodesCount;
        ++m_freqCount;

        adjust(newNode);
        m_root = newNode;

        return 1;
    }

    TreeNodePtr searchNode = m_root;
    TreeNodePtr parentNode = NULL;
    int level = 0;
    int child = 0; // 0 is left child, 1 is right child

    while (searchNode)
    {
        ++level;
        parentNode = searchNode;

        if (_value < searchNode->key)
        {
            searchNode = searchNode->left;
            child = 0;
        }
        else if (_value > searchNode->key)
        {
            searchNode = searchNode->right;
            child = 1;
        }
        else break;
    }

    if (searchNode)
    {
        searchNode->freq++;
        ++m_freqCount;
    }
    else
    {
        ++m_nodesCount;
        ++m_freqCount;
        TreeNodePtr newNode = new TreeNode_t;
        newNode->key  = _value;
        newNode->level = level;
        newNode->parent = parentNode;

        if (child == 0)
            parentNode->left = newNode;
        else
            parentNode->right = newNode;

        adjust(newNode);
        return 1;
    }

    return searchNode->freq;
}

template<typename KEY_T, typename DATA_T>
void CBSTree<KEY_T, DATA_T>::delNode(TreeNodePtr _node)
{
    if (!_node) return;
    delNode(_node->left);
    delNode(_node->right);
    delete _node;
}


template<typename KEY_T, typename DATA_T>
void CBSTree<KEY_T, DATA_T>::clear()
{
    delNode(m_root);
}

template<typename KEY_T, typename DATA_T>
typename CBSTree<KEY_T, DATA_T>::TreeNodePtr
CBSTree<KEY_T, DATA_T>::grandparent(TreeNodePtr _node)
{
    if (!_node || !_node->parent) return NULL;
    return _node->parent->parent;
}

template<typename KEY_T, typename DATA_T>
typename CBSTree<KEY_T, DATA_T>::TreeNodePtr
CBSTree<KEY_T, DATA_T>::uncle(TreeNodePtr _node)
{
    if (!_node) return NULL;

    TreeNodePtr gp = grandparent(_node);
    if (!gp)    return NULL;

    if (gp->parent == gp->left)
        return gp->right;
    else
        return gp->left;
}

template<typename KEY_T, typename DATA_T>
typename CBSTree<KEY_T, DATA_T>::TreeNodePtr
CBSTree<KEY_T, DATA_T>::sibling(TreeNodePtr _node)
{
    if (!_node || !_node->parent)   return NULL;

    if (_node == _node->parent->left)
        return _node->parent->right;
    else
        return _node->parent->left;
}

template<typename KEY_T, typename DATA_T>
typename CBSTree<KEY_T, DATA_T>::TreeNodePtr 
CBSTree<KEY_T, DATA_T>::findNode(const KEY_T& _value)
{
    TreeNodePtr searchNode = m_root;
    while (searchNode)
    {
        if (_value < searchNode->key)
            searchNode = searchNode->left;
        else if (_value > searchNode->key)
            searchNode = searchNode->right;
        else
            return searchNode;
    }

    return NULL;
}

template<typename KEY_T, typename DATA_T>
int CBSTree<KEY_T, DATA_T>::removeValue(const KEY_T& _value)
{
    TreeNodePtr node = findNode(_value);
    if (!node)  return -1;

    TreeNodePtr nextNode = NULL;
    TreeNodePtr parentNode = node->parent;

    if (node->left)    // 1. has left child
    {
        TreeNodePtr p = node->left;
        while (p->right)
            p = p->right;   // get the most right child as the placement node
        nextNode = p;

        if (nextNode->left)
            p = nextNode->left;
        while (p && p->left)   p = p->left;

        if (p)      // the most left child point to the the left child of the removed node
        {
            p->left = node->left;         
            node->left->parent = p;
        }

        nextNode->right = node->right;  // directly point to the right child of the removed node
        if (node->right)    node->right->parent = nextNode;

        if (nextNode->parent->right == nextNode)
            nextNode->parent->right = NULL;
        else
            nextNode->parent->left = NULL;
    }
    else
    {
        nextNode = node->right;
    }

    if (parentNode)
    {
        if (parentNode->left == node)
            parentNode->left = nextNode;
        else
            parentNode->right = nextNode;

        if (nextNode)   nextNode->parent = parentNode;
    }
    else
    {
        m_root = nextNode;
        if (nextNode)   nextNode->parent = NULL;
    }

    int freq = node->freq;
    delete node;
    --m_nodesCount;
    m_freqCount -= freq;

    return freq;
}

template<typename KEY_T, typename DATA_T>
int CBSTree<KEY_T, DATA_T>::delValue(const KEY_T& _value)
{
    TreeNodePtr node = findNode(_value);
    if (!node)  return -1;

    int freq = --node->freq;
    if (0 == freq)
        removeValue(_value);
    
    m_freqCount -= 1;

    return  freq;
}

template<typename KEY_T, typename DATA_T>
void CBSTree<KEY_T, DATA_T>::midOrder(TreeNodePtr _node, KEY_T* _buf, int& _n)
{
    if (!_node || !_buf)    return;
    if (_node->left)   midOrder(_node->left, _buf, _n);
    _buf[_n] = _node->key;
    ++_n;
    if (_node->right)   midOrder(_node->right, _buf, _n);
}

template<typename KEY_T, typename DATA_T>
bool CBSTree<KEY_T, DATA_T>::validate()
{
    //shared_ptr<T> sortArray(new T[m_nodeCount], default_delete<T[]>());
    std::unique_ptr<KEY_T[]> sortArray(new KEY_T[m_nodesCount]);

    KEY_T* sortArrayPtr = sortArray.get();
    int n = 0;
    midOrder(m_root, sortArrayPtr, n);

    for (int i = 0; i < m_nodesCount - 1; ++i)
    {
        if (sortArray[i] >= sortArray[i + 1])
            assert(false);

        cerr << sortArray[i] << ", ";
    }
    cerr << sortArray[m_nodesCount-1] << endl;

    return true;
}
