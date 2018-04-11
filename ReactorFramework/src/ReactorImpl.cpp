#include "Reactor.h"
#include "ReactorImpl.h"
#include "FunctorT.h"
#include "TimerQueueT.h"

ReactorImpl::~ReactorImpl( void )
{
}

DevPollReactorImpl::HandlerRepository::HandlerRepository( void )
    : m_Size( 0 ), m_MaxSize( 0 ), m_pEvtTupHandlers( NULL )
{
}

bool DevPollReactorImpl::HandlerRepository::HandleIsInvalid( HANDLE handle ) const
{
    if( handle < 0 || handle > m_MaxSize )
    {
        errno = EINVAL;
        return true;
    }
    return false;
}

int DevPollReactorImpl::HandlerRepository::Open( size_t size )
{
    this->m_MaxSize = size;

    NEW_RETURN( this->m_pEvtTupHandlers, EventTuple[size], -1 );

    return PubFunc::SetHandleLimit( size );
}

DevPollReactorImpl::EventTuple*
DevPollReactorImpl::HandlerRepository::Find( HANDLE handle )
{
    EventTuple *tuple = NULL;

    if( !this->HandleInRange(handle) )
    {
        cout<<"DevPollReactorImpl::HandlerRepository::Find out of range!"<<endl;
        errno = ERANGE;
        return NULL;
    }

    tuple = &( this->m_pEvtTupHandlers[handle] );
    if( NULL == tuple->m_pEvtHandler )
    {
        //cout<<"DevPollReactorImpl::HandlerRepository::Find tuple->m_pEvtHandler is null set errno=ENOENT"<<endl;
        //errno = ENOENT;
        tuple = NULL;
    }

    return tuple;
}

int DevPollReactorImpl::HandlerRepository::Bind( HANDLE handle, EventHandler *eventHandler, ReactorMask mask )
{
    if( NULL == eventHandler )
    {
        return -1;
    }

    if( INVALID_HANDLE == handle )
    {
        handle = eventHandler->GetHandle();
    }

    if( this->HandleIsInvalid(handle) )
    {
        return -1;
    }

    this->m_pEvtTupHandlers[handle].m_pEvtHandler = eventHandler;
    this->m_pEvtTupHandlers[handle].m_Mask = mask;
    eventHandler->AddReference();
    ++this->m_Size;

    return 0;
}

int DevPollReactorImpl::HandlerRepository::UnBind( HANDLE handle, bool bDecRefcnt )
{
    EventTuple *entry = this->Find( handle );
    if( NULL == entry )
    {
        errno = ENOENT;
        return -1;
    }
    
    if( bDecRefcnt )
    {
        entry->m_pEvtHandler->RemoveReference();
    }

    entry->m_pEvtHandler = NULL;
    entry->m_bIsSuspended = false;
    entry->m_bIsControlled = false;
    --this->m_Size;

    return 0;
}

int DevPollReactorImpl::HandlerRepository::UnBindAll( void )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::HandlerRepository::UnBindAll() Begin"<<endl;

    for( int handle=0; handle<this->m_MaxSize; ++handle )
    {
        EventTuple *entry = this->Find( handle );

        if( NULL == entry )
        {
            continue;
        }

        bool const bDecRefcntRequired = entry->m_pEvtHandler->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

        entry->m_pEvtHandler->HandleClose( handle, entry->m_Mask );

        this->UnBind( handle, bDecRefcntRequired );
    }

    return 0;
}

int DevPollReactorImpl::HandlerRepository::Close( void )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::HandlerRepository::Close() Begin"<<endl;

    if( this->m_pEvtTupHandlers != NULL )
    {
        this->UnBindAll();

        delete[] this->m_pEvtTupHandlers;
        this->m_pEvtTupHandlers = NULL;
    }
    return 0;
}

bool DevPollReactorImpl::HandlerRepository::HandleInRange( HANDLE handle ) const
{
    if( handle >= 0 && handle < this->m_MaxSize )
    {
        return true;
    }
    else
    {
        errno = EINVAL;
        return false;
    }
}

inline size_t DevPollReactorImpl::HandlerRepository::GetSize( void ) const
{
    return this->m_Size;
}
inline size_t DevPollReactorImpl::HandlerRepository::GetMaxSize( void ) const
{
    return this->m_MaxSize;
}

inline int DevPollReactorImpl::UpCall( EventHandler *eventHandler
                            , int (EventHandler::*CallBack)( HANDLE )
                            , HANDLE handle )
{
    int iStatus = 0;
    do
    {
        iStatus = (eventHandler->*CallBack)( handle );
    }
    while( iStatus > 0 );

    return iStatus;
}

DevPollReactorImpl::DevPollReactorImpl( TimerQueue *pTimerQueue )
    : m_bIsInitialized( false ), m_ePollFd( INVALID_HANDLE ), m_bRestart( false ), m_pTimerQueue( NULL )
{
    if( -1 == this->Open(PubFunc::GetMaxHandles(), false, pTimerQueue) )
    {
        cout<<"DevPollReactorImpl ctr failed"<<endl;
    }
}

DevPollReactorImpl::DevPollReactorImpl( size_t size, bool bRestart, TimerQueue *pTimerQueue )
{
    if( -1 == this->Open(size, bRestart, pTimerQueue) )
    {
        cout<<"DevPollReactorImpl ctr failed"<<endl;
    }
}

DevPollReactorImpl::~DevPollReactorImpl( void )
{
    this->Close();
}

int DevPollReactorImpl::Open( size_t size, bool bRestart, TimerQueue *pTimerQueue )
{
    if( this->m_bIsInitialized )
    {
        return -1;
    }

    memset( &this->m_ePollEvent, 0x00, sizeof(this->m_ePollEvent) );
    this->m_ePollEvent.data.fd = INVALID_HANDLE;
    this->m_bRestart = bRestart;
    this->m_pTimerQueue = pTimerQueue;

    int iRet = 0;

    if( NULL == this->m_pTimerQueue )
    {
        NEW_RETURN( this->m_pTimerQueue, TimerHeap, -1 );
        if( NULL == this->m_pTimerQueue )
        {
            iRet = -1;
        }
        else
        {
            this->m_bDelTimerQueue = true;
        }
    }

    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::Open epll_create: "<<size<<endl;
    this->m_ePollFd = epoll_create( size );
    if( -1 == this->m_ePollFd )
    {
        iRet = -1;
    }

    if( iRet != -1 && -1 == this->m_HandlerRepository.Open( size ) )
    {
        iRet = -1;
    }

    if( iRet != -1 )
    {
        this->m_bIsInitialized = true;
    }
    else
    {
        this->Close();
    }

    return iRet;
}

int DevPollReactorImpl::Close( void )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::Close() Begin"<<endl;

    int iRet = 0;

    if( this->m_ePollFd != INVALID_HANDLE )
    {
        iRet = close( this->m_ePollFd );
    }

    memset( &this->m_ePollEvent, 0x00, sizeof(this->m_ePollEvent) );
    this->m_ePollEvent.data.fd = INVALID_HANDLE;

    this->m_HandlerRepository.Close();

    this->m_ePollFd = INVALID_HANDLE;

    this->m_bIsInitialized = false;

    return iRet;
}

TimerQueue* DevPollReactorImpl::GetTimerQueue( void )
{
    return this->m_pTimerQueue;
}
int DevPollReactorImpl::SetTimerQueue( TimerQueue *timerQueue )
{
    if( this->m_bDelTimerQueue )
    {
        delete this->m_pTimerQueue;
    }
    else if( this->m_pTimerQueue != NULL )
    {
        this->m_pTimerQueue->Close();
    }

    this->m_pTimerQueue = timerQueue;
    this->m_bDelTimerQueue = false;

    return 0;
}

int DevPollReactorImpl::HandleEvents( TimeValue *maxWaitTime )
{
    TokenGuard guard( this->m_Token );
    
    return this->HandleEventsAux( maxWaitTime, guard );
}
int DevPollReactorImpl::AlertTableHandleEvents( TimeValue *maxWaitTime )
{
    return this->HandleEvents( maxWaitTime );
}
int DevPollReactorImpl::HandleEventsAux( TimeValue *maxWaitTime, TokenGuard &guard )
{
    int iRet = 0;

    do
    {
        iRet = this->WorkPendingAux( maxWaitTime );
        cout<<__FILE__<<"_"<<__LINE__<<"_"<<__FUNCTION__<<"_"<<"this->WorkPendingAux( "<<maxWaitTime<<" )="<<iRet<<endl;
        
        if( -1 == iRet && (!this->m_bRestart || errno != EINTR) )
        {
            cout<<"DevPollReactorImpl::HandleEventsAux WorkPendingAux error!"<<endl;
        }
    }
    while( -1 == iRet && this->m_bRestart && EINTR == errno );

    if( 0 == iRet || (-1 == iRet && ETIME == errno) )
    {
        return 0;
    }
    else if( -1 == iRet )
    {
        if( errno != EINTR )
        {
            return -1;
        }
        //will be add sig deal code
        return -1;
    }

    return this->Dispatch( guard );
}

int DevPollReactorImpl::GetDeactivated( void )
{
    return this->m_Deactivated;
}

void DevPollReactorImpl::SetDeactivate( int iDoStop )
{
    this->m_Deactivated = iDoStop;

    this->WakeupAllThreads();
}

int DevPollReactorImpl::RegisterHandler( EventHandler *eventHandler, ReactorMask mask )
{
    GUARD_RETURN( SYNCH_MUTEX, guard, this->m_HdlrRepoLock, -1 );

    return this->RegisterHandlerAux( eventHandler->GetHandle(), eventHandler, mask );
}

int DevPollReactorImpl::RegisterHandler( HANDLE ioHandle, EventHandler *eventHandler, ReactorMask mask )
{
    GUARD_RETURN( SYNCH_MUTEX, gurad, this->m_HdlrRepoLock, -1 );

    return this->RegisterHandlerAux( ioHandle, eventHandler, mask );
}

int DevPollReactorImpl::RegisterHandlerAux( HANDLE handle, EventHandler *eventHandler, ReactorMask mask )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::RegisterHandlerAux handle="<<handle<<" eventHandler="<<eventHandler<<" mask="<<mask<<endl;

    if( INVALID_HANDLE == handle || EventHandler::NULL_MASK == mask )
    {
        errno = EINVAL;
        return -1;
    }

    if( NULL == this->m_HandlerRepository.Find( handle ) )
    {
        if( this->m_HandlerRepository.Bind(handle, eventHandler, mask) != 0 )
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::RegisterHandlerAux m_HandlerRepository.Bind failed"<<endl;
            return -1;
        }

        EventTuple *evtTupInfo = this->m_HandlerRepository.Find( handle );

        struct epoll_event epEvent;
        memset( &epEvent, 0x00, sizeof(epEvent) );

        epEvent.data.fd = handle;
        epEvent.events = this->ReactorMaskToPollEvent( mask );
        epEvent.events |= EPOLLONESHOT;

        if( -1 == epoll_ctl(this->m_ePollFd, EPOLL_CTL_ADD, handle, &epEvent) )
        {
            this->m_HandlerRepository.UnBind( handle );
            return -1;
        }

        evtTupInfo->m_bIsControlled = true;
    }
    else
    {
        if( -1 == this->MaskOpsAux(handle, mask, Reactor::ADD_MASK) )
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::RegisterHandlerAux MaskOpsAux failed"<<endl;
        }
    }

    return 0;
}

int DevPollReactorImpl::RemoveHandler( EventHandler *eventHandler, ReactorMask mask )
{
    GUARD_RETURN( SYNCH_MUTEX, guard, this->m_HdlrRepoLock, -1 );

    return this->RemoveHandlerAux( eventHandler->GetHandle(), mask, guard );
}

int DevPollReactorImpl::RemoveHandler( HANDLE handle, ReactorMask mask )
{
    GUARD_RETURN( SYNCH_MUTEX, guard, this->m_HdlrRepoLock, -1 );

    return this->RemoveHandlerAux( handle, mask, guard );
}

int DevPollReactorImpl::RemoveHandlerAux( HANDLE handle
                                        , ReactorMask mask
                                        , SynchGuard<SYNCH_MUTEX> &hdrRepoGuard
                                        , EventHandler *eventHandler )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::RemoveHandlerAux handle="<<handle<<"mask="<<mask<<endl;

    bool handleRegChange = true;

    EventTuple *evtTupInfo = this->m_HandlerRepository.Find( handle );
    if( NULL == evtTupInfo && NULL == eventHandler )
    {
        errno = ENOENT;
        return -1;
    }
    
    if( evtTupInfo != NULL && ( NULL == eventHandler || evtTupInfo->m_pEvtHandler == eventHandler) )
    {
        if( -1 == this->MaskOpsAux(handle, mask, Reactor::CLR_MASK) )
        {
            return -1;
        }
        handleRegChange = false;
        eventHandler = evtTupInfo->m_pEvtHandler;
    }

    bool const bHdrRefCntRequired = eventHandler->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

    if( BIT_DISABLED(mask, EventHandler::DONT_CALL) )
    {
        hdrRepoGuard.Release();
        cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::RemoveHandlerAux eventHandler->HandleClose( "<<handle<<", "<<mask<<" )"<<endl;
        eventHandler->HandleClose( handle, mask );
        hdrRepoGuard.Acquire();
    }

    if( !handleRegChange && evtTupInfo->m_Mask == EventHandler::NULL_MASK )
    {
        this->m_HandlerRepository.UnBind( handle, bHdrRefCntRequired );
    }
    return 0;
}

int DevPollReactorImpl::SuspendHandler( EventHandler *eventHandler )
{
    if( NULL == eventHandler )
    {
        errno = EINVAL;
        return -1;
    }

    HANDLE handle = eventHandler->GetHandle();

    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->SuspendHandlerAux( handle );
}
int DevPollReactorImpl::SuspendHandler( HANDLE handle )
{
    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->SuspendHandlerAux( handle );
}
int DevPollReactorImpl::SuspendHandlerAux( HANDLE handle )
{
    EventTuple *info = this->m_HandlerRepository.Find( handle );
    if( NULL == info )
    {
        errno = ENOENT;
        return -1;
    }

    if( info->m_bIsSuspended )
        return 0;  // Already suspended.  @@ Should this be an error?

    // Remove the handle from the "interest set."
    //
    // Note that the associated event handler is still in the handler
    // repository, but no events will be polled on the given handle thus
    // no event will be dispatched to the event handler.

    struct epoll_event epEvent;
    memset( &epEvent, 0x00, sizeof(epEvent) );
    static const int op = EPOLL_CTL_DEL;

    epEvent.events  = 0;
    epEvent.data.fd = handle;

    if( -1 == epoll_ctl( this->m_ePollFd, op, handle, &epEvent) )
        return -1;
    info->m_bIsControlled = false;

    info->m_bIsSuspended= true;

    return 0;
}

int DevPollReactorImpl::ResumeHandler( EventHandler *eventHandler )
{
    if( NULL == eventHandler )
    {
        errno = EINVAL;
        return -1;
    }

    HANDLE handle = eventHandler->GetHandle();

    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->ResumeHandlerAux(handle);

}
int DevPollReactorImpl::ResumeHandler( HANDLE handle )
{
    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->ResumeHandlerAux(handle);
}
int DevPollReactorImpl::ResumeHandlerAux( HANDLE handle )
{
    EventTuple *evtTupInfo = this->m_HandlerRepository.Find( handle );
    if( evtTupInfo == NULL )
    {
        errno = ENOENT;
        return -1;
    }

    if( !evtTupInfo->m_bIsSuspended )
    {
        return 0;
    }

    ReactorMask mask = evtTupInfo->m_Mask;
    if(mask == EventHandler::NULL_MASK)
    {
        evtTupInfo->m_bIsSuspended = false;
        return 0;
    }

    // Place the handle back in to the "interest set."
    //
    // Events for the given handle will once again be polled.

    struct epoll_event epEvent;
    memset( &epEvent, 0, sizeof(epEvent) );
    int op = EPOLL_CTL_ADD;
    if( evtTupInfo->m_bIsControlled )
    {
        op = EPOLL_CTL_MOD;
    }
    epEvent.events  = this->ReactorMaskToPollEvent(mask) | EPOLLONESHOT;
    epEvent.data.fd = handle;

    if( -1 == epoll_ctl(this->m_ePollFd, op, handle, &epEvent) )
    {
        return -1;
    }
    evtTupInfo->m_bIsControlled = true;

    evtTupInfo->m_bIsSuspended = false;

    return 0;

}

long DevPollReactorImpl::ScheduleTimer( EventHandler *eventHandler
                                        , const void *arg
                                        , const TimeValue &tvDelay
                                        , const TimeValue &tvInterval )
{
    GUARD_RETURN( DevPollReactorToken, guard, this->m_Token, -1 );

    if( this->m_pTimerQueue != NULL )
    {
        return this->m_pTimerQueue->Schedule( eventHandler, arg, this->m_pTimerQueue->GetTimeOfDay()+tvDelay, tvInterval );
    }

    errno = ESHUTDOWN;
    return -1;
}

int DevPollReactorImpl::ResetTimerInterval( long lTimerId, const TimeValue &interval )
{
    GUARD_RETURN( DevPollReactorToken, guard, this->m_Token, -1 );

    if( NULL != this->m_pTimerQueue )
    {
        return this->m_pTimerQueue->ResetInterval( lTimerId, interval );
    }

    errno = ESHUTDOWN;
    return -1;
}

int DevPollReactorImpl::CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::CancelTimer this->m_pTimerQueue->Cancel(eventHandler, iDontCallHandleClose) Begin!"<<endl;
    return ( this->m_pTimerQueue == NULL
            ? NULL
            : this->m_pTimerQueue->Cancel(eventHandler, iDontCallHandleClose) );

}
int DevPollReactorImpl::CancelTimer( long lTimerId
                        , const void **pAct
                        , int iDontCallHandleClose )
{
    return ( this->m_pTimerQueue == NULL
            ? NULL
            : this->m_pTimerQueue->Cancel(lTimerId, pAct, iDontCallHandleClose) );
}

EventHandler* DevPollReactorImpl::FindHandler( HANDLE handle )
{
    EventTuple *info = this->m_HandlerRepository.Find( handle );
    if( info != NULL )
    {
        info->m_pEvtHandler->AddReference();
        return info->m_pEvtHandler;
    }
    else
    {
        return NULL;
    }
}

int DevPollReactorImpl::Handler( HANDLE handle, ReactorMask mask, EventHandler **eventHandler )
{
    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    EventTuple *info = this->m_HandlerRepository.Find( handle );

    if( info != NULL
       && BIT_CMP_MASK(info->m_Mask,
                           mask,  // Compare all bits in the mask
                           mask))
    {
        if( eventHandler != NULL )
        {
            *eventHandler = info->m_pEvtHandler;
        }
        return 0;
    }

    return -1;
}

bool DevPollReactorImpl::IsInitialized( void )
{
    return this->m_bIsInitialized;
}

size_t DevPollReactorImpl::GetSize( void ) const
{
    return this->m_HandlerRepository.GetSize();
}

bool DevPollReactorImpl::GetRestartFlag( void )
{
    GUARD_RETURN( DevPollReactorToken, mon, this->m_Token, false );

    return this->m_bRestart;
}
bool DevPollReactorImpl::SetRestartFlag( bool bRestart )
{
    GUARD_RETURN( DevPollReactorToken, mon, this->m_Token, false );

    bool current_value = this->m_bRestart;
    this->m_bRestart = bRestart;
    return current_value;
}

int DevPollReactorImpl::MaskOps( EventHandler *eventHandler, ReactorMask mask, int iOps )
{
    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->MaskOpsAux( eventHandler->GetHandle(), mask, iOps );
}
int DevPollReactorImpl::MaskOps( HANDLE handle, ReactorMask mask, int iOps )
{
    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );

    return this->MaskOpsAux( handle, mask, iOps );
}
int DevPollReactorImpl::MaskOpsAux( HANDLE handle, ReactorMask mask, int iOps )
{
    EventTuple *evtTupInfo = this->m_HandlerRepository.Find( handle );
    if( NULL == evtTupInfo )
    {
        errno = ENOENT;
        return -1;
    }

    ReactorMask const oldMask = evtTupInfo->m_Mask;
    ReactorMask newMask = oldMask;

    switch( iOps )
    {
        case Reactor::GET_MASK:
            // The work for this operation is done in all cases at the
            // begining of the function.
            return oldMask;

        case Reactor::CLR_MASK:
            CLR_BITS(newMask, mask);
            break;

        case Reactor::SET_MASK:
            newMask = mask;
            break;

        case Reactor::ADD_MASK:
            SET_BITS(newMask, mask);
            break;

        default:
            return -1;
    }

    /// Reset the mask for the given handle.
    evtTupInfo->m_Mask = newMask;

    // Only attempt to alter events for the handle from the
    // "interest set" if it hasn't been suspended. If it has been
    // suspended, the revised mask will take affect when the
    // handle is resumed. The exception is if all the mask bits are
    // cleared, we can un-control the fd now.
    if(!evtTupInfo->m_bIsSuspended || (evtTupInfo->m_bIsControlled && newMask == 0))
    {

        short const events = this->ReactorMaskToPollEvent( newMask );

        struct epoll_event epEvent;
        memset(&epEvent, 0, sizeof(epEvent));
        int op;

        // EventHandler::NULL_MASK ???
        if(newMask == 0)
        {
            op          = EPOLL_CTL_DEL;
            epEvent.events = 0;
        }
        else
        {
            op          = EPOLL_CTL_MOD;
            epEvent.events = events | EPOLLONESHOT;
        }

        epEvent.data.fd = handle;

        if( epoll_ctl(this->m_ePollFd, op, handle, &epEvent) == -1 )
        {
            // If a handle is closed, epoll removes it from the poll set
            // automatically - we may not know about it yet. If that's the
            // case, a mod operation will fail with ENOENT. Retry it as
            // an add. If it's any other failure, just fail outright.
            if(op != EPOLL_CTL_MOD || errno != ENOENT ||
                epoll_ctl(this->m_ePollFd, EPOLL_CTL_ADD, handle, &epEvent) == -1)
            {
                return -1;
            }
        }
        evtTupInfo->m_bIsControlled = (op != EPOLL_CTL_DEL);
    }

    return oldMask;
}

int DevPollReactorImpl::WorkPendingAux( TimeValue * maxWaitTime )
{
    if( this->m_ePollEvent.data.fd != INVALID_HANDLE )
    {
        return 1;
    }

    TimeValue resTimeout(0);
    TimeValue *thisTimeout = this->m_pTimerQueue->CalculateTimeout( maxWaitTime, &resTimeout );

    // Check if we have timers to fire.
    int const timersPending = ( (thisTimeout != NULL && NULL == maxWaitTime)
                                || (thisTimeout != NULL && maxWaitTime != NULL && *thisTimeout != *maxWaitTime)
                                ? 1 : 0);
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::WorkPendingAux timersPending="<<timersPending<<endl;

    long const timeout = ( thisTimeout == NULL ? -1 : static_cast<ulong>(thisTimeout->GetMilliSecond()) );
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::WorkPendingAux timeout="<<timeout<<endl;

    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::WorkPendingAux epoll_wait begin..."<<endl<<endl<<endl;
    int const iFds = epoll_wait( this->m_ePollFd, &this->m_ePollEvent, 1, static_cast<int>(timeout) );
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::WorkPendingAux Call_epoll_wait iFds="<<iFds<<"_timersPending="<<timersPending<<endl;

    return ( 0 == iFds && timersPending != 0 ? 1 : iFds );
}

short DevPollReactorImpl::ReactorMaskToPollEvent( ReactorMask mask )
{
    if( mask == EventHandler::NULL_MASK )
    {
        // No event.  Remove from interest set.
        return EPOLL_CTL_DEL;
    }

    short events = 0;

    // READ, ACCEPT, and CONNECT flag will place the handle in the
    // read set.
    if(BIT_ENABLED(mask, EventHandler::READ_MASK)
       || BIT_ENABLED(mask, EventHandler::ACCEPT_MASK)
       || BIT_ENABLED(mask, EventHandler::CONNECT_MASK))
    {
        SET_BITS(events, EPOLLIN);
    }

    // WRITE and CONNECT flag will place the handle in the write set.
    if(BIT_ENABLED(mask, EventHandler::WRITE_MASK)
       || BIT_ENABLED(mask, EventHandler::CONNECT_MASK))
    {
        SET_BITS(events, EPOLLOUT);
    }

    // EXCEPT flag will place the handle in the except set.
    if(BIT_ENABLED(mask, EventHandler::EXCEPT_MASK))
    {
        SET_BITS(events, EPOLLPRI);
    }

    return events;
}

int DevPollReactorImpl::DispatchTimerEvent( TokenGuard &guard )
{
    typedef MemberFunctionCommandT<TokenGuard> GuardRelease;

    GuardRelease release( guard, &TokenGuard::ReleaseToken );

    int iRet = this->m_pTimerQueue->ExpireSingle( release );
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::DispatchTimerEvent this->m_pTimerQueue->ExpireSingle() ="<<iRet<<endl;
    return iRet;
}

int DevPollReactorImpl::DispatchIOEvent( TokenGuard &guard )
{
    const __uint32_t inEvent = EPOLLIN;
    const __uint32_t outEvent = EPOLLOUT;
    const __uint32_t excEvent = EPOLLPRI;
    const __uint32_t errEvent = EPOLLHUP | EPOLLERR;

    const HANDLE handle = this->m_ePollEvent.data.fd;
    __uint32_t revents = this->m_ePollEvent.events;
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::DispatchIOEvent handle="<<handle<<"revents="<<revents<<endl;

    this->m_ePollEvent.data.fd = INVALID_HANDLE;
    this->m_ePollEvent.events = 0;

    if( handle != INVALID_HANDLE )
    {
        EventTuple *evtTupInfo = NULL;
        ReactorMask dispMask = 0;
        EventHandler *evtHdr = NULL;
        int (EventHandler::*CallBack)( HANDLE ) = NULL;
        bool bRestartResumeEvtHdr = false;

        {
            GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );
            evtTupInfo = this->m_HandlerRepository.Find( handle );
            if( NULL == evtTupInfo )
            {
                errno = ENOENT;
                return 0;
            }

            evtHdr = evtTupInfo->m_pEvtHandler;
            if( BIT_ENABLED(revents, outEvent) )
            {
                dispMask = EventHandler::WRITE_MASK;
                CallBack = &EventHandler::HandleOutput;

                CLR_BITS( revents, outEvent );
            }
            else if( BIT_ENABLED(revents, inEvent) )
            {
                dispMask = EventHandler::READ_MASK;
                CallBack = &EventHandler::HandleInput;

                CLR_BITS( revents, inEvent );
            }
            else if( BIT_ENABLED(revents, excEvent) )
            {
                dispMask = EventHandler::EXCEPT_MASK;
                CallBack = &EventHandler::HandleException;

                CLR_BITS( revents, excEvent );
            }
            else if( BIT_ENABLED(revents, errEvent) )
            {
                this->RemoveHandlerAux( handle, EventHandler::ALL_EVENTS_MASK, grd, evtHdr );

                return 1;
            }
            else
            {
                cout<<"DevPollReactorImpl::DispatchIOEvent unknown events"<<endl;
            }

            //if( evtHdr != this->m_NotifyHandler )
            {
                evtTupInfo->m_bIsSuspended = true;

                bRestartResumeEvtHdr = evtHdr->ResumeHandler() == EventHandler::REACTOR_RESUMES_HANDLER;
            }
            cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::DispatchIOEvent handle="<<handle<<"__mask="<<dispMask<<endl;
        }
        int iStatus = -1;
        {
            guard.ReleaseToken();

            iStatus = this->UpCall( evtHdr, CallBack, handle );
            cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::DispatchIOEvent this->UpCall iStatus="<<iStatus<<endl;

            if( 0 == iStatus )
            {
                if( bRestartResumeEvtHdr )
                {
                    GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, -1 );
                    evtTupInfo = this->m_HandlerRepository.Find( handle );
                    if( evtTupInfo != NULL && evtTupInfo->m_pEvtHandler == evtHdr )
                    {
                        cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::DispatchIOEvent this->ResumeHandlerAux(handle="<<handle<<")"<<endl;
                        return this->ResumeHandlerAux( handle );
                    }
                }
                return 1;
            }

            GUARD_RETURN( SYNCH_MUTEX, grd, this->m_HdlrRepoLock, 1 );
            evtTupInfo = this->m_HandlerRepository.Find( handle );
            if( evtTupInfo != NULL && evtTupInfo->m_pEvtHandler == evtHdr )
            {
                if( iStatus < 0 )
                {
                    this->RemoveHandlerAux( handle, dispMask, grd, evtHdr );
                }
            }
        }
        return 1;
    }
    return 0;
}

int DevPollReactorImpl::Dispatch( TokenGuard &guard )
{
    int iRet = -1;
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::Dispatch this->DispatchTimerEvent(guard) begin"<<endl;
    if( (iRet = this->DispatchTimerEvent(guard)) != 0 )
    {
        return iRet;
    }
    cout<<__FILE__<<"_"<<__LINE__<<"__DevPollReactorImpl::Dispatch this->DispatchIOEvent(guard) begin"<<endl;
    iRet = this->DispatchIOEvent( guard );

    return iRet;
}

int DevPollReactorImpl::Notify( EventHandler *eventHandler
                            , ReactorMask masks
                            , TimeValue *timeout )
{
    return 0;
}

void DevPollReactorImpl::WakeupAllThreads( void )
{
    this->Notify( 0, EventHandler::NULL_MASK, const_cast<TimeValue*>(&TimeValue::zero) );
}

inline DevPollReactorImpl::TokenGuard::TokenGuard( DevPollReactorToken &token )
    : m_Token( token ), m_Owner( 0 )
{
}
inline DevPollReactorImpl::TokenGuard::~TokenGuard( void )
{
    this->ReleaseToken();
}
inline int DevPollReactorImpl::TokenGuard::IsOwner( void )
{
    return this->m_Owner;
}
int DevPollReactorImpl::TokenGuard::Acquire( TimeValue *timeOut )
{
    int iRet = 0;
    if( timeOut != NULL )
    {
        TimeValue tv = PubFunc::GetTimeOfDay();
        tv += *timeOut;
        iRet = this->m_Token.Acquire( NULL, NULL, &tv );
    }
    else
    {
        iRet = this->m_Token.Acquire();
    }
    if( -1 == iRet )
    {
        if( ETIME == errno )
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    this->m_Owner = 1;

    return iRet;
}

inline void DevPollReactorImpl::TokenGuard::ReleaseToken( void )
{
    if( 1 == this->m_Owner )
    {
        this->m_Token.Release();
        this->m_Owner = 0;
    }
}





