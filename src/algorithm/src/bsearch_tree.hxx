
template<typename T>
CBSTree<T>::CBSTree()
{
    m_root = NULL;
    m_nodeCount = 0;
    m_freqCount = 0;
}

template<typename T>
CBSTree<T>::~CBSTree()
{
    clear();
}

template<typename T>
void CBSTree<T>::adjustHeight(BSTreeNode<T>* _node)
{
    if (!_node) return;

    while (_node)
    {
        BSTreeNode<T>* parent = _node->pNode;
        if (parent)
        {
            int childHeight = parent->getChildsHeight();

            if (parent->lNode == _node)
                parent->lHeight++;
            else
                parent->rHeight++;

            if (childHeight >= parent->getChildsHeight()) // no need notify above
                break;
        }

        _node = parent;
    }
}

template<typename T>
int CBSTree<T>::addValue(const T& _value)
{
    if (!m_root)
    {
        m_root = new BSTreeNode<T>;
        m_root->data = _value;
        ++m_nodeCount;
        ++m_freqCount;
        return 1;
    }

    BSTreeNode<T>* searchNode = m_root;
    BSTreeNode<T>* parentNode = NULL;
    int level = 0;
    int child = 0; // 0 is left child, 1 is right child

    while (searchNode)
    {
        ++level;
        parentNode = searchNode;

        if (_value < searchNode->data)
        {
            searchNode = searchNode->lNode;
            child = 0;
        }
        else if (_value > searchNode->data)
        {
            searchNode = searchNode->rNode;
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
        ++m_nodeCount;
        ++m_freqCount;
        BSTreeNode<T>* newNode = new BSTreeNode<T>;     
        newNode->data  = _value;
        newNode->level = level;
        newNode->pNode = parentNode;

        if (child == 0)
            parentNode->lNode = newNode;
        else
            parentNode->rNode = newNode;

        adjustHeight(newNode);
        return 1;
    }

    return searchNode->freq;
}

template<typename T>
void CBSTree<T>::delNode(BSTreeNode<T>* _node)
{
    if (!_node) return;
    delNode(_node->lNode);
    delNode(_node->rNode);
    delete _node;
}


template<typename T>
void CBSTree<T>::clear()
{
    delNode(m_root);
}

template<typename T>
BSTreeNode<T>* CBSTree<T>::findNode(const T& _value)
{
    BSTreeNode<T>* searchNode = m_root;
    while (searchNode)
    {
        if (_value < searchNode->data)
            searchNode = searchNode->lNode;
        else if (_value > searchNode->data)
            searchNode = searchNode->rNode;
        else
            return searchNode;
    }

    return NULL;
}

template<typename T>
int CBSTree<T>::removeValue(const T& _value)
{
    BSTreeNode<T>* node = findNode(_value);
    if (!node)  return -1;

    BSTreeNode<T>* nextNode     = NULL;
    BSTreeNode<T>* parentNode   = node->pNode;

    if (node->lNode)    // 1. has left child
    {
        BSTreeNode<T>* p = node->lNode;
        while (p->rNode)
            p = p->rNode;   // get the most right child as the placement node
        nextNode = p;

        if (nextNode->lNode)
            p = nextNode->lNode;
        while (p && p->lNode)   p = p->lNode;

        if (p)      // the most left child point to the the left child of the removed node
        {
            p->lNode = node->lNode;         
            node->lNode->pNode = p;
        }

        nextNode->rNode = node->rNode;  // directly point to the right child of the removed node
        if (node->rNode)    node->rNode->pNode = nextNode;

        if (nextNode->pNode->rNode == nextNode)
            nextNode->pNode->rNode = NULL;
        else
            nextNode->pNode->lNode = NULL;
    }
    else
    {
        nextNode = node->rNode;
    }

    if (parentNode)
    {
        if (parentNode->lNode == node)
            parentNode->lNode = nextNode;
        else
            parentNode->rNode = nextNode;

        if (nextNode)   nextNode->pNode = parentNode;
    }
    else
    {
        m_root = nextNode;
        if (nextNode)   nextNode->pNode = NULL;
    }

    int freq = node->freq;
    delete node;
    --m_nodeCount;
    m_freqCount -= freq;

    return freq;
}

template<typename T>
int CBSTree<T>::delValue(const T& _value)
{
    BSTreeNode<T>* node = findNode(_value);
    if (!node)  return -1;

    int freq = --node->freq;
    if (0 == freq)
        removeValue(_value);
    
    m_freqCount -= 1;

    return  freq;
}

template<typename T>
void CBSTree<T>::midOrder(BSTreeNode<T>* _node, T* _buf, int& _n)
{
    if (!_node || !_buf)    return;
    if (_node->lNode)   midOrder(_node->lNode, _buf, _n);
    _buf[_n] = _node->data;
    ++_n;
    if (_node->rNode)   midOrder(_node->rNode, _buf, _n);
}

template<typename T>
bool CBSTree<T>::validate()
{
    //shared_ptr<T> sortArray(new T[m_nodeCount], default_delete<T[]>());
    std::unique_ptr<T[]> sortArray(new T[m_nodeCount]);

    T* sortArrayPtr = sortArray.get();
    int n = 0;
    midOrder(m_root, sortArrayPtr, n);

    for (int i = 0; i < m_nodeCount - 1; ++i)
    {
        if (sortArray[i] >= sortArray[i + 1])
            assert(false);

        cerr << sortArray[i] << ", ";
    }
    cerr << sortArray[m_nodeCount-1] << endl;

    return true;
}
