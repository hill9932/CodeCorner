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
 * @Function: This task do nothing except decide when to stop
 **/
class CWatchTask : public CSerialTask
{
public:
    CWatchTask()
    {
        m_stop = false;
    }

    void* operator()(void* _item)
    {
        if (m_stop) return NULL;
        return _item;
    }

    void stop()
    {
        m_stop = true;
    }

private:
    bool m_stop;
};


/**
 * @Function: run the tbb pipeline
 */
class CTaskManager : public ISingleton<CTaskManager>
{
public:
    CTaskManager();

    bool start();   // start the pipeline
    bool stop();    // stop the pipeline
    void join();

    void addTask(CTask& _task);

private:
    void threadFunc();

private:
    tbb::pipeline   m_pipeline;
    std::thread     m_thread;
    CWatchTask      m_watchTask;
};

#endif
