#ifndef __HL_THREAD_H_INCLUDE__
#define __HL_THREAD_H_INCLUDE__

#include "log_.h"
#include <thread>

namespace LabSpace
{
    namespace Common
    {
        class CCurrentThread
        {
        public:
            /**
             * @Function: get the thread id as string
             **/
            static const tchar* ID();
            static std::thread::id id();

        private:
            static thread_local tchar t_tidString[32];
        };
    }
}

#endif
