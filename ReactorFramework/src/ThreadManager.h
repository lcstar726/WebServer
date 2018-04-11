#ifndef __THREAD_MANAGER_H__
#define __THREAD_MANAGER_H__

#include "Public.h"
#include "GlobalMacros.h"
#include "SynchTraits.h"
#include "FreeList.h"
#include "Containers.h"
#include "TimeValue.h"

class TaskBase;
class ThreadAdapter;
class ThreadDescriptor;

class ThreadManager
{
public:
    enum
    {
        // Uninitialized.
        THREAD_IDLE = 0x00000000,

        // Created but not yet running.
        THREAD_SPAWNED = 0x00000001,

        // Thread is active
        THREAD_RUNNING = 0x00000002,

        // Thread is suspended.
        THREAD_SUSPENDED = 0x00000004,

        // Thread has been cancelled
        THREAD_CANCELLED = 0x00000008,

        // Thread has shutdown
        THREAD_TERMINATED = 0x00000010,

        // Join operation has been invoked
        THREAD_JOINING = 0x10000000
    };
    
    ThreadManager( size_t preaolloc = DEFAULT_THREAD_MANAGER_PREALLOC,
                       size_t lwm = DEFAULT_THREAD_MANAGER_LWM,
                       size_t inc = DEFAULT_THREAD_MANAGER_INC,
                       size_t hwm = DEFAULT_THREAD_MANAGER_HWM );

    ~ThreadManager( void );
    
    static ThreadManager *GetInstance( void );
    //static ThreadManager *SetInstance( ThreadManager * );
    
    int Open( size_t size = 0 );
    int Close( void );
    
    int SpawnAux(THR_FUNC func
              , void *arg = 0
              , long flags = 0
              , thread_t *thrId = 0
              , hthread_t *thrHandle = 0
              , long priority = DEFAULT_THREAD_PRIORITY
              , int iGrpId = -1
              , void *stack = 0
              , size_t stackSize = 0
              , TaskBase *taskBase = 0
              , const char** thrName = 0);

    int SpawnN(size_t threadCnt
                , THR_FUNC func
                , void *arg = 0
                , long flags = THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED
                , long thrPriority = DEFAULT_THREAD_PRIORITY
                , int iGrpId = -1
                , TaskBase *taskBase = 0
                , hthread_t threadHandles[] = 0
                , void *stack[] = 0
                , size_t stackSize[] = 0
                , const char* thrName[] = 0);

    int AppendThread( thread_t thrId
                    , hthread_t thrHandle
                    , unsigned int thrState
                    , int grpId
                    , TaskBase *task = 0
                    , long flags = 0
                    , ThreadDescriptor *thrDesc = 0 );
                
    THR_FUNC_RETURN exit( THR_FUNC_RETURN status = 0, bool doThreadExit = true );
    
    int Wait( const TimeValue *tv = 0
            , bool bAbandonDetachedThreads = false
            , bool bUseAbsoluteTime = true );
            
    int Join( thread_t tid, THR_FUNC_RETURN *status = 0 );
    
protected:
    ThreadMutex m_Lock;
    int m_AutoWait;
    int m_GroupId;

    DoubleLinkedList<ThreadDescriptor> m_ThrList;

    LockedFreeList<ThreadDescriptor, SYNCH_MUTEX> m_ThrDescFreeList;
    
private:
    static ThreadManager *m_ThrMgr;
    
    static bool m_MustDelThrMgr;
    
};

class Thread
{
public:
    static int spawn( THR_FUNC func
                    , void *args = 0
                    , long flags = THR_NEW_LWP| THR_JOINABLE
                    , thread_t *thrId = 0
                    , hthread_t *thrHandle = 0
                    , long priority = DEFAULT_THREAD_PRIORITY
                    , void *thrStack = 0
                    , size_t stackSize = DEFAULT_THREAD_STACKSIZE
                    , ThreadAdapter *thrAdapter = 0
                    , const char** thrName = 0 );

    static int Join( thread_t thrId, thread_t *departed, THR_FUNC_RETURN *status );

    static int Resume( thread_t );

    static int Suspend( thread_t );

private:
    Thread( void );
};

#endif

