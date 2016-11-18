#ifndef __HL_BUFFER_INCLUDE_H__
#define __HL_BUFFER_INCLUDE_H__

#include "common.h"


namespace LabSpace
{
    namespace Common
    {
        template<int SIZE>
        class FixedBuffer : boost::noncopyable
        {
        public:
            FixedBuffer() : m_cur(m_data)
            {
                bzero(m_data, sizeof m_data);
            }

            ~FixedBuffer()
            {
            }

            void append(const char* _buf, size_t len)
            {
                if (avail() > len)
                {
                    memcpy(m_cur, _buf, len);
                    m_cur += len;
                }
            }

            const char* data() const    { return m_data; }
            char* current()             { return m_cur; }

            int length() const { return static_cast<int>(m_cur - m_data); }
            int avail()  const { return static_cast<int>(end() - m_cur - 1);  }
            int size()   const { return SIZE; }

            void add(size_t len) { m_cur += len; }
            void reset() { m_cur = m_data; /*bzero(m_data, sizeof m_data); */}

        private:
            const char* end() const { return m_data + sizeof m_data; }

        private:
            char    m_data[SIZE];
            char*   m_cur;
        };
    }
}

#endif
