#ifndef __HL_LOCKER_INCLUDE_H__
#define __HL_LOCKER_INCLUDE_H__

#include <atomic>
#include <mutex>
#include <condition_variable>


#define SCOPED_GUARD(lock)  std::lock_guard<std::mutex> guard(lock)

#endif

