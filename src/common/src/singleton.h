#ifndef __HL_SINGLETON_INCLUDE_H__
#define __HL_SINGLETON_INCLUDE_H__

#include "common.h"


namespace LabSpace
{
    namespace Common
    {
        /**
        * @Function: reference count
        * @Memo:    it is not thread safe
        **/
        class CCountRef
        {
        public:
            CCountRef()
            {
                m_nRef = 0;
            }

            int addRef()
            {
                ++m_nRef;
                return m_nRef;
            }

            bool releaseRef()
            {
                if (m_nRef <= 0) return false;

                --m_nRef;
                if (m_nRef == 0) 
                {
                    onDestroy();
                    return true;
                }

                return false;
            }

            int getRefCount()
            {
                return m_nRef;
            }

        protected:
            virtual ~CCountRef()  {}         // protect object from being deleted explicitly
            virtual void onDestroy() { delete this; }

        private:
            int    m_nRef;
        };


        /**
        * @Function: the base class to act as singleton
        **/
        template <class T>
        class ISingleton : public CCountRef
        {
        public:
            static T* GetInstance()
            {
                SCOPED_GUARD(s_instMutex);

                if (!s_instance)
                    s_instance = new T;
                
                s_instance->addRef();

                return s_instance;
            }

            void release()
            {
                SCOPED_GUARD(s_instMutex);
                releaseRef();
            }

            void destroy()
            {
                SCOPED_GUARD(s_instMutex);
                while (!releaseRef());
            }

        protected:
            ISingleton() {}
            virtual ~ISingleton() {};
            virtual void onDestroy()
            {
                delete this;
                s_instance = NULL;
            }

        private:
            static std::mutex   s_instMutex;
            static T*           s_instance;
        };

        template<typename T> std::mutex ISingleton<T>::s_instMutex;
        template<typename T> T* ISingleton<T>::s_instance = NULL;

    }
}

#endif
