#ifndef __HL_MEMORY_CREATOR_INCLUDE_H__
#define __HL_MEMORY_CREATOR_INCLUDE_H__

#include <memory>

class NonAllocable
{
private:
    NonAllocable()   {}
    ~NonAllocable()  {}

private:
    template<typename> friend class CMemCreator;
    template<typename> friend class CMemDeleter;
};


template<class T>
class NoneDeleter
{
public:
    void operator()(T*) {}
};


template<class T>
class CMemDeleter : public ISingleton<CMemDeleter<T> >
{
public:
    typedef shared_ptr<T>   MemPtr;

    void operator()(T* p)
    {
        m_memPool.push_back(MemPtr(p, NoneDeleter<T>()));
    }

    void swap(vector<MemPtr>& _v)
    {
        _v.swap(m_memPool);
    }

private:
    vector<MemPtr>  m_memPool;
};


template<class T>
class CMemCreator : public ISingleton<CMemCreator<T> >
{
public:
    typedef shared_ptr<T>   MemPtr;

    bool init()
    {
        for (int i = 0; i < s_poolSize; ++i)
        {
            MemPtr memPtr(new T, *CMemDeleter<T>::GetInstance());   // specify CMemDelete<T> to delete the memory
            m_memPool.push_back(memPtr);
        }

        return true;
    }

    MemPtr create()
    {
        SCOPED_GUARD(m_poolMutex);

        if (m_memPool.size() == 0)
        {
            CMemDeleter<T>::GetInstance()->swap(m_memPool);
            if (m_memPool.size() == 0)
                return MemPtr();
        }

        MemPtr elem = m_memPool.back();
        m_shadowMemPool.push_back(elem);
        m_memPool.pop_back();

        return elem;
    }

private:
    vector<MemPtr>  m_memPool;
    vector<MemPtr>  m_shadowMemPool;
    std::mutex      m_poolMutex;

    static const int s_poolSize = 100;
};


#endif
