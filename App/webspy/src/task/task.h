#ifndef __HL_TASK_INCLUDE_H__
#define __HL_TASK_INCLUDE_H__

#include "common.h"
#include "singleton.h"
#include <tbb/pipeline.h>
#include <thread>


class CTask : public tbb::filter
{
public:
    CTask(bool _isSerial) : filter(_isSerial)
    {}

    bool virtual init() { return true; }
};


/**
 * @Function: Base serial tbb task
 */
class CSerialTask : public CTask
{
public:
    CSerialTask() : CTask(true)   {   }
};


/**
 * @Function: Base parallel tbb task
 */
class CParallelTask : public CTask
{
public:
    CParallelTask() : CTask(false)   {   }
};


/**
 * @Function: run the tbb pipeline
 */
class CTaskManager : public ISingleton<CTaskManager>
{
public:
    CTaskManager();

    bool start();   // start the pipeline
    void stop()     { m_stop = true; }
    bool isStop()   { return m_stop; }
    void join();

    void addTask(CTask& _task);

private:
    void threadFunc();

private:
    tbb::pipeline   m_pipeline;
    std::thread     m_thread;
    bool            m_stop;
};

#endif
