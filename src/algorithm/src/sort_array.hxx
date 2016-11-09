template<typename DATA_T, int SIZE>
CSortArray<DATA_T, SIZE>::CSortArray()
{
    m_nodes.reset(new ArrayNode_t[SIZE]);
}

template<typename DATA_T, int SIZE>
CSortArray<DATA_T, SIZE>::~CSortArray()
{

}
