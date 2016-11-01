template<typename KEY, typename DATA, int SIZE>
CHeap<KEY, DATA, SIZE>::CHeap()
{
    m_nodes.reset(new NodeType[SIZE]);
    m_nodesCount = 0;
}

template<typename KEY, typename DATA, int SIZE>
CHeap<KEY, DATA, SIZE>::~CHeap()
{

}

template<class T>
bool IsBigger(const T& _l, const T& _r)
{
    return _l > _r;
}

template<class T>
bool IsSmaller(const T& _l, const T& _r)
{
    return _l < _r;
}

template<typename KEY, typename DATA, int SIZE>
void CHeap<KEY, DATA, SIZE>::exchangeNodes(int _l, int _r)
{
    KEY  lKey  = m_nodes[_l].key;
    DATA lData = m_nodes[_l].data;
    m_nodes[_l].key  = m_nodes[_r].key;
    m_nodes[_l].data = m_nodes[_r].data;
    m_nodes[_r].key  = lKey;
    m_nodes[_r].data = lData;
}

template<typename KEY, typename DATA, int SIZE>
void CHeap<KEY, DATA, SIZE>::maxOrMinHeap(int _index, CompFunc _func)
{
    const int MaxIndex = m_nodesCount - 1;
    int left, right, largest;

    largest = left = 2 * _index + 1;
    right   = left + 1;
    if (left > MaxIndex)    return;

    if (right <= MaxIndex && 
        _func(m_nodes[right].key, m_nodes[left].key))
        largest = right;

    if (_func(m_nodes[largest].key, m_nodes[_index].key))
    {
        exchangeNodes(_index, largest);
        maxOrMinHeap(largest, _func);
    }
}

template<typename KEY, typename DATA, int SIZE>
void CHeap<KEY, DATA, SIZE>::createMaxHeap()
{
    for (int i = m_nodesCount / 2 - 1; i >= 0; --i)
        maxOrMinHeap(i, IsBigger<KEY>);
}

template<typename KEY, typename DATA, int SIZE>
void CHeap<KEY, DATA, SIZE>::createMinHeap()
{
    for (int i = m_nodesCount / 2 - 1; i >= 0; --i)
        maxOrMinHeap(i, IsSmaller<KEY>);
}

template<typename KEY, typename DATA, int SIZE>
int CHeap<KEY, DATA, SIZE>::addNode(const NodeType* _node)
{
    if (!_node || m_nodesCount == SIZE)  return -1;

    m_nodes[m_nodesCount].key  = _node->key;
    m_nodes[m_nodesCount].data = _node->data;
    ++m_nodesCount;
    return 0;
}

template<typename KEY, typename DATA, int SIZE>
int CHeap<KEY, DATA, SIZE>::addNode(const NodeType& _node)
{
    return addNode(&_node);
}

template<typename KEY, typename DATA, int SIZE>
void CHeap<KEY, DATA, SIZE>::sortHeap()
{
    int i;
    createMaxHeap();
    for (i = m_nodesCount - 1; i >= 1; i--)
    {
        exchangeNodes(i, 0);
        maxOrMinHeap(0, IsBigger<KEY>);
    }
}

template<typename KEY, typename DATA, int SIZE>
bool CHeap<KEY, DATA, SIZE>::validate()
{
    for (int i = 0; i < m_nodesCount; ++i)
    {
        cerr << m_nodes[i].key << ", ";
    }
    cerr << endl;

    return true;
}