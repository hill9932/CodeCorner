#include "thread_.h"

namespace LabSpace
{
    namespace Common
    {
        thread_local tchar CCurrentThread::t_tidString[32];
        
        std::thread::id id()
        {
            return std::this_thread::get_id();
        }

        /**
         * @Function: get the thread id as 
         **/
        const tchar* CCurrentThread::ID()
        {
            const tchar* id = t_tidString;
            if (!*id)
            {
                void* p = &std::this_thread::get_id();
                t_tidString[0] = '0';
                t_tidString[1] = 'x';
                StrUtil::convertHex(t_tidString + 2, p);
            }
            
            return id;
        }
    }
}
