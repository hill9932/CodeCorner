#ifndef __HL_TASK_INCLUDE_H__
#define __HL_TASK_INCLUDE_H__

#include "common.h"
#include "singleton.h"
#include <tbb/pipeline.h>
#include <thread>
#include <atomic>

class CTask : public tbb::filter
{
public:
    CTask(bool _isSerial) : filter(_isSerial)
    {
        m_issueCount = 0;
    }

    bool virtual init() { return true; }

protected:
    std::atomic<u_int64>    m_issueCount;
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
class CPipeline : public ISingleton<CPipeline>
{
public:
    CPipeline();

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


/**
 * @Function: this pipeline is used to parsing the web pages
 **/
class CProcessPipeline : public CPipeline
{

};


/**
 * @Function: this pipeline is used to download web pages
 **/
class CCollectPipeline : public CPipeline
{

};
#endif
