#include "task.h"


CTaskManager::CTaskManager()
{
    m_stop = false;
}

void CTaskManager::addTask(CTask& _task)
{
    m_pipeline.add_filter(_task);
}

#define MAX_TOKEN_COUNT         64
void CTaskManager::threadFunc()
{
    m_pipeline.run(MAX_TOKEN_COUNT);
}

bool CTaskManager::start()
{
    if (m_thread.get_id() != std::thread::id())   return true;

    m_thread = std::thread(&CTaskManager::threadFunc, this);
    return m_thread.joinable();
}

void CTaskManager::join()
{
    m_thread.join();
}