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


template<class T> class CMemCreator;


template<class T>
class CMemDeleter : public ISingleton<CMemDeleter<T> >
{
private:
    CMemDeleter() {}

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
    friend class ISingleton<CMemDeleter<T>>;
    friend class CMemCreator<T>;
};


template<class T>
class CMemCreator : public ISingleton<CMemCreator<T> >
{
private:
    CMemCreator() {}

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
            m_shadowMemPool.push_back(memPtr);
        }
        
        return true;
    }

    MemPtr create()
    {
  /*      MemPtr memPtr(new T, m_deletor);   // specify CMemDelete<T> to delete the memory
        m_shadowMemPool.push_back(memPtr);
        return memPtr;
        */
        SCOPED_GUARD(m_poolMutex);

        if (m_memPool.size() == 0)
        {
            cleanup();

            m_deletor.swap(m_memPool);
            if (m_memPool.size() == 0)
                return MemPtr();
        }

        MemPtr elem = m_memPool.back();
        m_memPool.pop_back(); 

        return elem;        
    }

    void cleanup()
    {
        typename list<MemPtr>::const_iterator it = m_shadowMemPool.begin();
        for (; it != m_shadowMemPool.end(); ++it)
        {
            if ((*it)->finished)
            {
                assert((*it).use_count() == 1);
                it = m_shadowMemPool.erase(it);
            }
        }
    }

private:
    vector<MemPtr>  m_memPool;
    list<MemPtr>    m_shadowMemPool;
    std::mutex      m_poolMutex;

    CMemDeleter<T>  m_deletor;

    static const int s_poolSize = 100;
    friend class ISingleton<CMemCreator<T> >;
};


#endif
