#include "task.h"


CPipeline::CPipeline()
{
    m_stop = false;
}

void CPipeline::addTask(CTask& _task)
{
    m_pipeline.add_filter(_task);
}

#define MAX_TOKEN_COUNT         64
void CPipeline::threadFunc()
{
    m_pipeline.run(MAX_TOKEN_COUNT);
}

bool CPipeline::start()
{
    if (m_thread.get_id() != std::thread::id())   return true;

    m_thread = std::thread(&CPipeline::threadFunc, this);
    return m_thread.joinable();
}

void CPipeline::join()
{
    m_thread.join();
}