template<typename KEY_T, typename DATA_T, int SIZE>
CHeap<KEY_T, DATA_T, SIZE>::CHeap()
{
    m_nodes.reset(new HeapNode_t[SIZE]);
    m_nodesCount = 0;
}

template<typename KEY_T, typename DATA_T, int SIZE>
CHeap<KEY_T, DATA_T, SIZE>::~CHeap()
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

template<typename KEY_T, typename DATA_T, int SIZE>
void CHeap<KEY_T, DATA_T, SIZE>::exchangeNodes(int _l, int _r)
{
    if (_l == _r)   return;

    KEY_T  lKey  = m_nodes[_l].key;
    DATA_T lData = m_nodes[_l].data;
    m_nodes[_l].key  = m_nodes[_r].key;
    m_nodes[_l].data = m_nodes[_r].data;
    m_nodes[_r].key  = lKey;
    m_nodes[_r].data = lData;
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::partition(int _left, int _right, CompFunc _func)
{
    if (_left  < 0 ||
        _right < 0 ||
        _left  >= m_nodesCount ||
        _right >= m_nodesCount)
        return -1;

    KEY_T pivot = m_nodes[_right].key;
    int pos = _left - 1;
    for (int i = _left; i < _right; ++i)
    {
        if (_func(m_nodes[i].key, pivot))
            exchangeNodes(++pos, i);
    }
    exchangeNodes(++pos, _right);
    return pos;
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::choosePivotByRandom(int _left, int _right, int _k)
{
    return LabSpace::Common::Util::Random(_left, _right);
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::choosePivotByMedian(int _left, int _right, int _k)
{
    if (_k - 1 > _right || _k - 1 < _left)
        return -1;

    int midIndex = (_left + _right) / 2;
    if (m_nodes[_left].key < m_nodes[midIndex].key)
        exchangeNodes(_left, midIndex);

    if (m_nodes[_right].key < m_nodes[midIndex].key)
        exchangeNodes(_right, midIndex);

    if (m_nodes[_right].key < m_nodes[_left].key)
        exchangeNodes(_right, _left);

    return _left;
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::choosePivotByDirect(int _left, int _right, int _k)
{
    return _right;
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CHeap<KEY_T, DATA_T, SIZE>::selectMaxN(int _left, int _right, int _k)
{
    int index = choosePivotByRandom(_left, _right, _k);
    exchangeNodes(index, _right);
    int pos = partition(_left, _right, IsBigger<KEY_T>);
    if (pos == _k - 1)
        return true;
    else if (pos > _k - 1)
        return selectMaxN(_left, pos - 1, _k);
    else
        return selectMaxN(pos + 1, _right, _k);
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CHeap<KEY_T, DATA_T, SIZE>::selectMaxN(int _k)
{
    return selectMaxN(0, m_nodesCount - 1, _k);
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CHeap<KEY_T, DATA_T, SIZE>::selectMinN(int _left, int _right, int _k)
{
    int index = choosePivotByMedian(_left, _right, _k);
    exchangeNodes(index, _right);
    int pos = partition(_left, _right, IsSmaller<KEY_T>);
    if (pos == _k - 1)
        return true;
    else if (pos > _k - 1)
        return selectMinN(_left, pos - 1, _k);
    else
        return selectMinN(pos + 1, _right, _k);
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CHeap<KEY_T, DATA_T, SIZE>::selectMinN(int _k)
{
    return selectMinN(0, m_nodesCount - 1, _k);
}

template<typename KEY_T, typename DATA_T, int SIZE>
void CHeap<KEY_T, DATA_T, SIZE>::maxOrMinHeap(int _index, CompFunc _func)
{
    const int MaxIndex = m_nodesCount - 1;
    int left, right, largest;

    largest = left = 2 * _index + 1;
    right   = left + 1;
    if (left > MaxIndex)    return;

    if (right <= MaxIndex && 
        _func(m_nodes[right].key, m_nodes[left].key))       // find the bigger/smaller one for max heap
        largest = right;

    if (_func(m_nodes[largest].key, m_nodes[_index].key))   // when parent smaller/bigger than the children in max heap
    {
        exchangeNodes(_index, largest);
        maxOrMinHeap(largest, _func);       // from top to down to adjust again
    }
}

template<typename KEY_T, typename DATA_T, int SIZE>
void CHeap<KEY_T, DATA_T, SIZE>::createMaxHeap()
{
    for (int i = m_nodesCount / 2 - 1; i >= 0; --i)     // from down to top 
        maxOrMinHeap(i, IsBigger<KEY_T>);
}

template<typename KEY_T, typename DATA_T, int SIZE>
void CHeap<KEY_T, DATA_T, SIZE>::createMinHeap()
{
    for (int i = m_nodesCount / 2 - 1; i >= 0; --i)
        maxOrMinHeap(i, IsSmaller<KEY_T>);
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::addNode(const HeapNode_t* _node)
{
    if (!_node || m_nodesCount == SIZE)  return -1;

    m_nodes[m_nodesCount].key  = _node->key;
    m_nodes[m_nodesCount].data = _node->data;
    ++m_nodesCount;
    return 0;
}

template<typename KEY_T, typename DATA_T, int SIZE>
int CHeap<KEY_T, DATA_T, SIZE>::addNode(const HeapNode_t& _node)
{
    return addNode(&_node);
}

template<typename KEY_T, typename DATA_T, int SIZE>
void CHeap<KEY_T, DATA_T, SIZE>::sortHeap()
{
    int keepCount = m_nodesCount;

    int i;
    createMaxHeap();
    for (i = m_nodesCount - 1; i >= 1; i--)
    {
        exchangeNodes(i, 0);
        --m_nodesCount;     // the biggest is in the last position, so skip it
        maxOrMinHeap(0, IsBigger<KEY_T>);
    }
    m_nodesCount = keepCount;
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CHeap<KEY_T, DATA_T, SIZE>::validate()
{
    cerr << endl;
    for (int i = 0; i < m_nodesCount; ++i)
    {
        cerr << m_nodes[i].key << ", ";
    }
    cerr << endl;

    return true;
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CTopNHeap<KEY_T, DATA_T, SIZE>::validate()
{
    CHeap::validate();

    for (int i = 0; i < m_nodesCount; ++i)
    {
        if (IsBigger(m_nodes[0].key, m_nodes[i].key))
        {
            cerr << "Exception" << endl;
        }
        assert(m_nodes[0].key <= m_nodes[i].key);
    }
    return true;
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CTopNHeap<KEY_T, DATA_T, SIZE>::checkNode(const HeapNode_t* _node)
{
    if (!_node) return false;

    bool r = false;
    if (m_nodesCount != SIZE)
    {
        addNode(_node);
        createMinHeap();

        r = true;
    }
    else if (IsBigger(_node->key, m_nodes[0].key))
    {
        m_nodes[0].key  = _node->key;
        m_nodes[0].data = _node->data;
        maxOrMinHeap(0, IsSmaller<KEY_T>);    // since replace the [0] node, so only adjust the [0] node

        r = true;
    }

    return r;
}

template<typename KEY_T, typename DATA_T, int SIZE>
bool CTopNHeap<KEY_T, DATA_T, SIZE>::checkNode(const HeapNode_t& _node)
{
    return checkNode(&_node);
}