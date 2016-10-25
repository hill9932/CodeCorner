#ifndef __HL_MEMORY_CREATOR_INCLUDE_H__
#define __HL_MEMORY_CREATOR_INCLUDE_H__

#include "singleton.h"
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
#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
        L4C_LOG_DEBUG("Deleter get " << p->debugId);
#endif
        p->reset();

        m_memPool.push_back(MemPtr(p, *this));
    }

    void swap(vector<MemPtr>& _v)
    {
        _v.swap(m_memPool);
    }

    int getRespCount() { return m_memPool.size(); }

private:
    static vector<MemPtr>  m_memPool;
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
            T* obj = new T;

#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
            obj->setDebugId(i);
#endif
            MemPtr memPtr(obj, m_deletor);   // specify CMemDelete<T> to delete the memory
            m_memPool.push_back(memPtr);
        }

        return true;
    }

    MemPtr create()
    {
        SCOPED_GUARD(m_poolMutex);

        if (m_memPool.size() == 0)
        {
            cleanup();

            m_deletor.swap(m_memPool);
            if (m_memPool.size() == 0)
                return MemPtr();
        }

        MemPtr elem = m_memPool.back();
        m_shadowMemPool.push_back(elem);
        m_memPool.pop_back();
        
#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
        int n = m_deletor.getRespCount();
        assert(n + m_shadowMemPool.size() + m_memPool.size() == s_poolSize);
        L4C_LOG_DEBUG("Creator return " << elem->debugId);
#endif

        return elem;
    }

    void cleanup()
    {
        list<MemPtr>::const_iterator it = m_shadowMemPool.begin();
        for (; it != m_shadowMemPool.end(); ++it)
        {
            if ((*it)->finished)
            {
                assert((*it).use_count() == 1);

                it = m_shadowMemPool.erase(it);

#if defined(ENABLE_DEBUG_MEM_POOL_ASSERT)
                int n2 = m_deletor.getRespCount();
                int m2 = m_shadowMemPool.size();
                assert(n2 + m2 == s_poolSize);
#endif
            }
        }
    }

private:
    vector<MemPtr>  m_memPool;
    list<MemPtr>    m_shadowMemPool;
    std::mutex      m_poolMutex;

    CMemDeleter<T>  m_deletor;

    static const int s_poolSize = 100;
};


#endif
