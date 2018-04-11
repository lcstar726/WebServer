#include "TaskBase.h"
#include "SynchTraits.h"
#include "ThreadManager.h"

TaskBase::TaskBase( ThreadManager *thrMgr )
    : m_ThreadCount( 0 ), m_ThreadMgr( thrMgr ), m_Flags( 0 ), m_GroupId( -1 ), m_LastThreadId( 0 )
{
}

TaskBase::~TaskBase( void )
{
}

int TaskBase::Activate( long flags
                        , int iThrCount
                        , int iForceActive
                        , long thrPriority
                        , int iGroupId
                        , TaskBase *taskBase
                        , hthread_t threadHandls[]
                        , void *stack[]
                        , size_t stackSize[]
                        , thread_t threadIds[]
                        , const char* threadName[] )
{
    GUARD_RETURN( ThreadMutex, monitor, this->m_Lock, -1 );
    
    if( 0 == taskBase )
    {
        taskBase = this;
    }
    
    if( iThrCount > 0 && 0 == iForceActive )
    {
        return 1;
    }
    else
    {
        if( this->m_ThreadCount > 0 && this->m_GroupId != -1 )
        {
            iGroupId = this->m_GroupId;
            
            this->m_ThreadCount += iThrCount;
        }
    }
    
    if( 0 == this->m_ThreadMgr )
    {
        this->m_ThreadMgr = ThreadManager::GetInstance();
    }
    
    int iGrpSpawned = -1;
    
    if( 0 == threadIds )
    {
        iGrpSpawned = this->m_ThreadMgr->SpawnN( iThrCount
                                                , &TaskBase::SvcRun
                                                , (void*)this
                                                , flags
                                                , thrPriority
                                                , iGroupId
                                                , taskBase
                                                , threadHandls
                                                , stack
                                                , stackSize
                                                , threadName );
    }
    else
    {
        //iGrpSpawned = this->m_ThreadMgr->SpawnN( 
    }
    
    if( -1 == iGrpSpawned )
    {
        this->m_ThreadCount -= iThrCount;
        return -1;
    }
    
    if( -1 == this->m_GroupId )
    {
        this->m_GroupId = iGrpSpawned;
    }
    
    this->m_LastThreadId = 0;
    
    return 0;
}

int TaskBase::Open( void *args )
{
    return 0;
}

int TaskBase::Close( unsigned long flags )
{
    return 0;
}

int TaskBase::Service( void )
{
    return 0;
}

THR_FUNC_RETURN TaskBase::SvcRun( void *args )
{
    TaskBase *taskBase = static_cast<TaskBase*>( args );

    int const svcStatus = taskBase->Service();

    THR_FUNC_RETURN status = reinterpret_cast<THR_FUNC_RETURN>(svcStatus);

    return status;
}
