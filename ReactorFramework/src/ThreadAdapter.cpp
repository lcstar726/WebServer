#include "ThreadAdapter.h"
#include "ThreadManager.h"
#include "OSThread.h"

ThreadDescriptorBase::ThreadDescriptorBase( void )
    : OSThreadDescriptor(), m_ThreadId( OS::nullThreadId ), m_ThreadHandle( OS::nullThreadHandle )
    , m_GroupId( 0 ), m_CurrentState( ThreadManager::THREAD_IDLE )
    , m_Task( 0 ), m_Next( 0 ), m_Prev( 0 )
{
}

int ThreadDescriptorBase::GetGroupId( void ) const
{
    return this->m_GroupId;
}

unsigned int ThreadDescriptorBase::GetCurrentState( void ) const
{
    return this->m_CurrentState;
}

TaskBase* ThreadDescriptorBase::GetTask( void ) const
{
    return this->m_Task;
}

void ThreadDescriptorBase::Reset( void )
{
    this->m_ThreadId = OS::nullThreadId;
    this->m_ThreadHandle = OS::nullThreadHandle;
    this->m_GroupId = 0;
    this->m_CurrentState = ThreadManager::THREAD_IDLE;
    this->m_Task = 0;
    this->m_Next = 0;
    this->m_Prev = 0;
}

ThreadDescriptor::ThreadDescriptor( void )
    : m_ThrMgr( 0 ), m_IsTerminated( false )
{
    NEW( this->m_Synch, DEFAULT_THREAD_MANAGER_LOCK );
}

ThreadDescriptor::~ThreadDescriptor( void )
{
    delete this->m_Synch;
}

thread_t ThreadDescriptor::GetSelfThreadId( void )
{
    return this->m_ThreadId;
}

void ThreadDescriptor::SetNext( ThreadDescriptor *thrDesc )
{
    this->m_Next = thrDesc;
}
ThreadDescriptor* ThreadDescriptor::GetNext( void ) const
{
    return static_cast<ThreadDescriptor *>( this->m_Next );
}

void ThreadDescriptor::Reset( ThreadManager *thrMgr )
{
    ThreadDescriptorBase::Reset();

    this->m_ThrMgr = thrMgr;

    this->m_IsTerminated = false;
}

void ThreadDescriptor::Acquire( void )
{
    this->m_Synch->Acquire();
}
void ThreadDescriptor::Release( void )
{
    this->m_Synch->Release();
}

ThreadAdapterBase::ThreadAdapterBase( THR_FUNC UserFunc
                        , void *args
                        , THR_C_FUNC EntryPoint
                        , OSThreadDescriptor *thrDesc
                        , long cancelFlags )
    : mfp_UserFunc( UserFunc ), m_Args( args ), mfp_EntryPoint( EntryPoint ), m_OSThrDesc( thrDesc ), m_CancelFlags( cancelFlags )
{
}

THR_C_FUNC ThreadAdapterBase::GetEntryPoint( void )
{
    return this->mfp_EntryPoint;
}

OSThreadAdapter::OSThreadAdapter( THR_FUNC UserFunc
                                , void *args
                                , THR_FUNC EntryPoint
                                , long cancelFlags )
    :ThreadAdapterBase( UserFunc, args, EntryPoint, 0, cancelFlags )
{
}

ThreadAdapter::ThreadAdapter( THR_FUNC UserFunc
                    , void *args
                    , THR_C_FUNC EntryPoint
                    , ThreadManager *thrMgr
                    , ThreadDescriptor *thrDesc
                    , long cancelFlags )
    : ThreadAdapterBase( UserFunc, args, EntryPoint, thrDesc , cancelFlags ), m_ThrMgr( thrMgr )
{
}

THR_FUNC_RETURN ThreadAdapter::Invoke( void )
{
    return this->InvokeAux();
}

THR_FUNC_RETURN ThreadAdapter::InvokeAux( void )
{
    THR_FUNC Func = reinterpret_cast<THR_FUNC>( this->mfp_UserFunc );
    
    void *args = this->m_Args;
    
    long cancelFlags = this->m_CancelFlags;
    
    delete this;
    
    THR_FUNC_RETURN status = 0;
    
    status = (*Func)( args );
    
    return status;
}

ThreadManager* ThreadAdapter::GetThreadMgr( void )
{
    return this->m_ThrMgr;
}

extern "C"
THR_FUNC_RETURN THREAD_ADAPTER_NAME( void *args )
{
    ThreadAdapterBase * const thrArgs = static_cast<ThreadAdapterBase*>( args );
    
    THR_FUNC_RETURN status = thrArgs->Invoke();
    
    return status;
}

