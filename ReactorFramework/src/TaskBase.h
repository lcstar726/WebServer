#ifndef __TASK_BASE_H__
#define __TASK_BASE_H__

#include "base.h"
#include "GlobalMacros.h"
#include "ThreadMutex.h"

class ThreadManager;
class TaskBase
{
public:
    TaskBase( ThreadManager *thrMgr = 0 );
    
    virtual ~TaskBase( void );
    
    virtual int Activate( long flags = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED
                        , int iThrCount = 1
                        , int iForceActive = 0
                        , long thrPriority = DEFAULT_THREAD_PRIORITY
                        , int iGroutId = -1
                        , TaskBase *taskBase = 0
                        , hthread_t threadHandls[] = 0
                        , void *stack[] = 0
                        , size_t stackSize[] = 0
                        , thread_t threadIds[] = 0
                        , const char* threadName[] = 0 );
    
    virtual int Open( void *args = 0 );
    
    virtual int Close( unsigned long flags = 0 );
    
    virtual int Service( void );

    static THR_FUNC_RETURN SvcRun( void * );
    
protected:
    size_t m_ThreadCount;
    
    ThreadManager *m_ThreadMgr;
    
    unsigned long m_Flags;
    
    int m_GroupId;
    
    ThreadMutex m_Lock;
    
    thread_t m_LastThreadId;
    
private:
    TaskBase( const TaskBase & );
    TaskBase& operator= ( const TaskBase & );
};

#endif

