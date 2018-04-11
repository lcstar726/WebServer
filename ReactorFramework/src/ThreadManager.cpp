#include "OSThread.h"
#include "ThreadManager.h"
#include "ObjectManager.h"
#include "UniquePtr.h"

ThreadManager *ThreadManager::m_ThrMgr = 0;
bool ThreadManager::m_MustDelThrMgr = false;

ThreadManager::ThreadManager (size_t prealloc,
                                        size_t lwm,
                                        size_t inc,
                                        size_t hwm)
    :m_GroupId( 1 ), m_AutoWait( 1 )
{
}

ThreadManager::~ThreadManager( void )
{
    this->Close();
}

int ThreadManager::Close( void )
{
    return 0;
}

int ThreadManager::Open( size_t size )
{
    return 0;
}

ThreadManager* ThreadManager::GetInstance( void )
{
    cout<<"ThreadManager::GetInstance( void )"<<endl;
    
    if( 0 == ThreadManager::m_ThrMgr )
    {
        GUARD_RETURN( RecursiveThreadMutex, monitor, *StaticObjectLock::GetInstance(), 0 );
        
        if( 0 == ThreadManager::m_ThrMgr )
        {
            NEW_RETURN( ThreadManager::m_ThrMgr, ThreadManager, 0 );
            
            ThreadManager::m_MustDelThrMgr = true;
        }
    }
    return ThreadManager::m_ThrMgr;
}
int ThreadManager::AppendThread( thread_t thrId
                , hthread_t thrHandle
                , unsigned int thrState
                , int grpId
                , TaskBase *task
                , long flags
                , ThreadDescriptor *thrDesc )
{
    ThreadDescriptor *tmpThrDesc = 0;
    if( 0 == thrDesc )
    {
        NEW_RETURN( tmpThrDesc, ThreadDescriptor, -1 );
        tmpThrDesc->m_ThrMgr = this;
    }
    else
    {
        tmpThrDesc = thrDesc;
    }
    tmpThrDesc->m_ThreadId = thrId;
    tmpThrDesc->m_ThreadHandle = thrHandle;
    tmpThrDesc->m_GroupId = grpId;
    tmpThrDesc->m_Task = task;
    tmpThrDesc->m_ThreadFlags = flags;

    this->m_ThrList.InsertHead( tmpThrDesc );
    SET_BITS( tmpThrDesc->m_CurrentState, thrState );
    tmpThrDesc->m_Synch->Release();

    return 0;
}

int ThreadManager::SpawnN(size_t threadCnt
                , THR_FUNC func
                , void *args
                , long flags
                , long thrPriority
                , int iGrpId
                , TaskBase *taskBase
                , hthread_t threadHandles[]
                , void *stack[]
                , size_t stackSize[]
                , const char* thrName[] )
{
    if( -1 == iGrpId )
    {
        iGrpId = this->m_GroupId++;
    }
    
    for( int i=0; i<(int)threadCnt; ++i )
    {
        if( -1 == SpawnAux( func
                            , args
                            , flags
                            , 0
                            , threadHandles == 0 ? 0 : &threadHandles[i]
                            , thrPriority
                            , iGrpId
                            , stack == 0 ? 0 : stack[i]
                            , stackSize == 0 ? 0 : stackSize[i]
                            , taskBase
                            , thrName == 0 ? 0 : &thrName[i] ) )
        {
            return -1;
        }
    }
    return iGrpId;
}

int ThreadManager::SpawnAux(THR_FUNC func
                              , void *args
                              , long flags
                              , thread_t *thrId
                              , hthread_t *thrHandle
                              , long priority
                              , int iGrpId
                              , void *stack
                              , size_t stackSize
                              , TaskBase *taskBase
                              , const char** thrName )
{
    UniquePtr<ThreadDescriptor> uniqueThrDesc( this->m_ThrDescFreeList.Remove() );

    uniqueThrDesc->Reset( this );

    ThreadAdapter *thrArgs = 0;
    NEW_RETURN( thrArgs, ThreadAdapter( func
                                        , args
                                        , (THR_C_FUNC)THREAD_ADAPTER_NAME
                                        , this
                                        , uniqueThrDesc.GetNativePtr()
                                        , flags ), -1 );

    UniquePtr<ThreadAdapterBase> uniqueThrArgs( static_cast<ThreadAdapterBase *>(thrArgs) );

    thread_t threadId;
    hthread_t threadHandle;

    if( 0 == thrId )
    {
        thrId = &threadId;
    }

    uniqueThrDesc->m_Synch->Acquire();

    int iResult = Thread::spawn( func, args, flags, thrId, &threadHandle
                                , priority, stack, stackSize, thrArgs, thrName );
    if( iResult != 0 )
    {
        uniqueThrDesc->Release();
        return -1;
    }
    uniqueThrArgs.Release();
    
    if( threadHandle != 0 )
    {
        *thrHandle = threadHandle;
    }

    return this->AppendThread( *thrId
                              , threadHandle
                              , ThreadManager::THREAD_SPAWNED
                              , iGrpId
                              , taskBase
                              , flags
                              , uniqueThrDesc.Release() );
}

int Thread::spawn( THR_FUNC func
                        , void *args
                        , long flags
                        , thread_t *thrId
                        , hthread_t *thrHandle
                        , long priority
                        , void *thrStack
                        , size_t stackSize
                        , ThreadAdapter *thrAdapter
                        , const char** thrName )
{
    return OS::ThreadCreate (func,
                               args,
                               flags,
                               thrId,
                               thrHandle,
                               priority,
                               thrStack,
                               stackSize,
                               thrAdapter,
                               thrName);

}
