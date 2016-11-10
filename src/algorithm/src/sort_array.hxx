template<typename KEY_T, int SIZE>
CSortArray<KEY_T, SIZE>::CSortArray()
{
    m_nodes.reset(new ArrayNode_t[SIZE]);
    m_nodesCount = 0;
}

template<typename KEY_T, int SIZE>
CSortArray<KEY_T, SIZE>::~CSortArray()
{

}

template<typename KEY_T, int SIZE>
int CSortArray<KEY_T, SIZE>::addValue(const KEY_T& _value)
{
    if (m_nodesCount == SIZE)   return -1;

    m_nodes[m_nodesCount++].key = _value;
    return 0;
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::exchange(int _l, int _r)
{
    if (_l >= m_nodesCount ||
        _r >= m_nodesCount)
        return;

    KEY_T tmp = m_nodes[_l].key;
    m_nodes[_l].key = m_nodes[_r].key;
    m_nodes[_r].key = tmp;
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::reverse(int _begin, int _end)
{
    while (_begin < _end)
    {
        exchange(_begin, _end);
        ++_begin;
        --_end;
    }
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::reverse()
{
    reverse(0, end());
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::cycle(int _shiftNum)
{
    if (_shiftNum == 0 ||
        m_nodesCount < 2)   return;

    _shiftNum %= m_nodesCount;

    if (_shiftNum > 0)
    {
        int split = end() - _shiftNum;
        reverse(0, split);
        reverse(split + 1, end());
        reverse(0, end());
    }
    else
    {
        int split = 0 - _shiftNum - 1;
        reverse(0, split);
        reverse(split + 1, end());
        reverse(0, end());
    }
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::shift(ArrayNodePtr _node, int _begin, int _end, int _shiftNum)
{
    if (_shiftNum == 0)   return;
    else if (_shiftNum > 0)
    {
        for (int i = _end; i >= _begin; --i)
        {
            int pos = i + _shiftNum;
            if (pos >= SIZE) pos = SIZE - 1;
            _node[pos].key = _node[i].key;
        }
    }
    else
    {
        for (int i = _begin; i <= _end; ++i)
        {
            int pos = i + _shiftNum;
            if (pos < 0)    pos = 0;
            _node[pos].key = _node[i].key;
        }
    }
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::shift(int _shiftNum)
{
    shift(m_nodes.get(), 0, getNodesCount(), _shiftNum);
    m_nodesCount += _shiftNum;
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::sort()
{
    std::unique_ptr<ArrayNode_t[]> nodes(new ArrayNode_t[SIZE]);
    for (int i = 0; i < m_nodesCount; i++)
    {
        int j = 0;
        for (j = 0; j < i; ++j)
        {
            if (m_nodes[i].key < nodes[j].key)
            {
                shift(nodes.get(), j, i - 1, 1);
                break;
            }
        }

        nodes[j].key = m_nodes[i].key;
    }
    m_nodes = std::move(nodes);
}

template<typename KEY_T, int SIZE>
KEY_T CSortArray<KEY_T, SIZE>::operator [](int _index)
{
    if (_index >= m_nodesCount)
        throw "out of sequence.";

    return m_nodes[_index].key;
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::validate(ArrayNodePtr _node)
{
    for (int i = 0; i < m_nodesCount; i++)
    {
        cerr << _node[i].key << ", ";
    }
    cerr << endl;
}

template<typename KEY_T, int SIZE>
void CSortArray<KEY_T, SIZE>::validate()
{
    validate(m_nodes.get());
}
