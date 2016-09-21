/**
 * @Function:
 *  This file provides a sorted array
 * @Memo:
 *  Create by hill, 26/6/2014
 **/
#ifndef __VP_SORT_ARRAY_INCLUDE_H__
#define __VP_SORT_ARRAY_INCLUDE_H__

#include "common.h"
#include <algorithm>

enum SA_INSERT_STAT
{
    SA_NOT_INSERT, SA_INSERT_WITHOUT_REMOVE, SA_INSERT_WITH_REMOVE_LAST
};

template<class T, class CompT>
class CSortArray
{
public:
    CSortArray(u_int32 _size = 0)
    {
        m_buffer = NULL;
        m_used = 0;

        if (_size > 0)
        {
            m_buffer = new T[_size];
            m_size = _size;
        }
        if (m_buffer == NULL)
        {
            m_size = 0;
        }
    }

    ~CSortArray()
    {
        if (m_buffer)   delete[] m_buffer;
    }

    void setSize(u_int32 _size)
    {
		m_used = 0;
		if (_size == m_size)
		{
			return;
		}

        if (m_buffer)
        {
            delete[] m_buffer;
            m_buffer = NULL;
        }

        if (_size > 0)
        {
            m_buffer = new T[_size];
            m_size = _size;
        }
        if (m_buffer == NULL)
        {
            m_size = 0;
        }
    }

    void clear()
    {
        m_used = 0;
        //m_buffer[m_used] = NULL;
    }

    bool insert(const T& _elem)
    {
        int pos = -1;
        if (find(_elem, &pos))
            return true;

        if (pos >= m_size)  return false;

        if (m_used > pos)
            memmove(&m_buffer[pos + 1], &m_buffer[pos], sizeof(T) * MyMin((m_used - pos), m_size - pos - 1));
        m_buffer[pos] = _elem;
        if (++m_used > m_size) m_used = m_size;

        return true;
    }

    SA_INSERT_STAT insertAndGetRemove(T& _elem)
    {
        SA_INSERT_STAT stat = SA_NOT_INSERT;
        int pos = -1;
        if (find(_elem, &pos))
            return stat;

        if (pos >= m_size)  return stat;

        T removeElem;
        if (m_used > pos)
        {
            if (m_used == m_size)
            {
                stat = SA_INSERT_WITH_REMOVE_LAST;
                removeElem = m_buffer[m_used - 1];
            } 
            else
            {
                stat = SA_INSERT_WITHOUT_REMOVE;
            }
            memmove(&m_buffer[pos + 1], &m_buffer[pos], sizeof(T)* MyMin((m_used - pos), m_size - pos - 1));
        }
        else
        {
            stat = SA_INSERT_WITHOUT_REMOVE;
        }
        m_buffer[pos] = _elem;
        if (stat == SA_INSERT_WITH_REMOVE_LAST)
        {
            _elem = removeElem;
        }
        if (++m_used > m_size) m_used = m_size;

        return stat;
    }
    
    bool find(const T& _elem, int* _pos = NULL)
    {
        if (m_used == 0)    
        { 
            if (_pos)   *_pos = 0;
            return false; 
        }
 /*       else if (m_used == m_size)  // quickly fail while the buffer is full
        {
            if (!m_comp(_elem, m_buffer[m_used - 1]))   // suppose sort descending, and is less than the last element
            {
                if (_pos)  *_pos = m_size;
                return (_elem == m_buffer[m_used - 1]) ? true : false;
            }
            else if (m_comp(_elem, m_buffer[0]))        // suppose sort descending, and is bigger than the first element
            {
                if (_pos)   *_pos = 0;
                return (_elem == m_buffer[0]) ? true : false;
            }
        }
        */
        int start = 0, stop = m_used - 1;
        int pos = 0;

        for (; start <= stop; )
        {
            pos = (start + stop) / 2;

            const T& e = m_buffer[pos];
            if (e == _elem)
            {
                if (_pos)   *_pos = pos;
                return true;
            }
            else if (m_comp(_elem, e))
                stop = pos - 1;
            else 
                start = pos + 1;
        }

        if (!m_comp(_elem, m_buffer[pos]))   ++pos;

        if (_pos)   *_pos = pos;

        return false;
    }

    int getSize() { return m_used;  }

    T*  getAt(int _index)
    {
        if (_index >= m_used)   return NULL;
        return &m_buffer[_index];
    }

    bool isFull()
    {
        return m_used == m_size;
    }

    bool isEmpty()
    {
        return m_used == 0;
    }

    bool remove(const T& _elem)
    {
        int pos = -1;
        if (!find(_elem, &pos))
            return false;

        --m_used;
        if (pos >= m_used - 1)  return true;

        memmove(&m_buffer[pos], &m_buffer[pos+1], sizeof(T) * (m_used - pos));
        return true;
    }

private:
    T*  m_buffer;
    int m_size;
    int m_used;
    CompT m_comp;
};

#endif
