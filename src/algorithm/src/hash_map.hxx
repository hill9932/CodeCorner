#include "hash.hxx"


template<typename DATA>
CHashMap<DATA>::CHashMap()
{
    HashNodePtr* nodes = new HashNodePtr[HASHLEN];
    bzero(nodes, sizeof(nodes) * HASHLEN);
    m_nodes.reset(nodes);
    m_nodesCount = 0;
}

template<typename DATA>
CHashMap<DATA>::~CHashMap()
{
    for (int i = 0; i < HASHLEN; ++i)
    {
        HashNodePtr node = m_nodes[i];
        while (node)
        {
            HashNodePtr tmpNode = node;
            node = node->next;

            SAFE_DELETE(tmpNode);
        }
    }
}

template<typename DATA>
typename CHashMap<DATA>::HashNodePtr CHashMap<DATA>::findNode(const DATA& _data)
{
    size_t hashValue = BKDRHash(_data.c_str());
    size_t bucket = hashValue % HASHLEN;
    HashNodePtr node = m_nodes[bucket];

    while (node)
    {
        if (_data == node->data)
        {
            break;
        }
        node = node->next;
    }

    return node;
}

template<typename DATA>
int CHashMap<DATA>::addNode(const DATA& _data)
{
    HashNodePtr node = findNode(_data);
    if (!node)
    {
        size_t hashValue = BKDRHash(_data.c_str());
        size_t bucket = hashValue % HASHLEN;

        node = new HashNode_t;
        node->count = 0;
        node->data = _data;
        node->next = m_nodes[bucket];

        m_nodes[bucket] = node;
    }

    ++m_nodesCount;
    return ++node->count;
}

template<typename DATA>
int CHashMap<DATA>::delNode(const DATA& _data)
{
    HashNodePtr node = findNode(_data);
    if (!node)  return -1;

    --m_nodesCount;

    if (0 == --node->count)
        return removeNode(_data);

    return node->count;
}

template<typename DATA>
int CHashMap<DATA>::removeNode(const DATA& _data)
{
    size_t hashValue = BKDRHash(_data.c_str());
    size_t bucket = hashValue % HASHLEN;
    HashNodePtr node = m_nodes[bucket];
    HashNodePtr parentNode = NULL;
    int nodeCount = 0;

    if (!node)  return -1;
    else
    {
        while (node)
        {
            if (node->data == _data)
                break;

            parentNode = node;
            node = node->next;
        }

        if (!node)  return -1;

        if (parentNode) parentNode->next = node->next;
        else m_nodes[bucket] = node->next;

        nodeCount = node->count;
        SAFE_DELETE(node);
    }

    m_nodesCount -= nodeCount;
    return 0;
}
