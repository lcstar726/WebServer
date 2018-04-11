#include "Reactor.h"
#include "ReactorImpl.h"
#include "EventHandler.h"

Reactor*    Reactor::m_pReactor = NULL;
bool        Reactor::m_bDelReactor = false;

ReactorTimerInterface::~ReactorTimerInterface( void )
{
}

Reactor::Reactor( ReactorImpl *impl, bool bDelImplemetation )
    : m_pReactorImpl( NULL ), m_bDelImplemetation( bDelImplemetation )
{
    this->SetImplemetation( impl );

    if( NULL == this->GetImplementation() )
    {
        NEW( impl, DevPollReactorImpl );

        this->SetImplemetation( impl );

        this->m_bDelImplemetation = true;
    }
}

Reactor::~Reactor( void )
{
    this->GetImplementation()->Close();

    if( this->m_bDelImplemetation )
    {
        delete this->GetImplementation();
    }
}

Reactor* Reactor::GetInstance( void )
{
    if( NULL == Reactor::m_pReactor )
    {
        GUARD_RETURN( RecursiveThreadMutex, guard, *StaticObjectLock::GetInstance(), 0 );

        if( NULL == Reactor::m_pReactor )
        {
            NEW_RETURN( Reactor::m_pReactor, Reactor, 0 );

            Reactor::m_bDelReactor = true;
        }
    }
    return Reactor::m_pReactor;
}

int Reactor::RunReactorEventLoop( REACTOR_EVENT_HOOK hookFunc )
{
    if( this->ReactorEventLoopDone() )
    {
        return 0;
    }

    for( ;; )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"_"<<__FUNCTION__<<"_"<<"this->m_pReactorImpl->HandleEvents() Begin"<<endl;
        int const iRet = this->m_pReactorImpl->HandleEvents();
        cout<<__FILE__<<"_"<<__LINE__<<"_"<<__FUNCTION__<<"_"<<"this->m_pReactorImpl->HandleEvents()="<<iRet<<endl<<endl<<endl;

        if( hookFunc != NULL && hookFunc(this) )
        {
            continue;
        }
        else if( iRet == -1 && this->m_pReactorImpl->GetDeactivated() )
        {
            return 0;
        }
        else if( iRet == -1 )
        {
            return -1;
        }
    }
    return 0;
}

int Reactor::EndReactorEventLoop( void )
{
    this->m_pReactorImpl->SetDeactivate( 1 );

    return 0;
}

int Reactor::ReactorEventLoopDone( void )
{
    return this->m_pReactorImpl->GetDeactivated();
}

int Reactor::Open( size_t iMaxHandlersNumber, bool bRestart )
{
    return this->m_pReactorImpl->Open( iMaxHandlersNumber, bRestart );
}

TimerQueue*  Reactor::GetTimerQueue( void )
{
    return this->m_pReactorImpl->GetTimerQueue();
}
int  Reactor::SetTimerQueue( TimerQueue *timerQueue )
{
    return this->m_pReactorImpl->SetTimerQueue( timerQueue );
}

int Reactor::Close( void )
{
    return this->m_pReactorImpl->Close();
}

int Reactor::HandleEvents( TimeValue *maxWaitTime )
{
    return this->m_pReactorImpl->HandleEvents( maxWaitTime );
}

int Reactor::RegisterHandler( EventHandler *eventHandler, ReactorMask mask )
{
    Reactor *oldReactor = eventHandler->GetReactor();

    eventHandler->SetReactor( this );

    int iRet = this->m_pReactorImpl->RegisterHandler( eventHandler, mask );
    cout<<__FILE__<<"_"<<__LINE__<<"__Reactor::RegisterHandler m_pReactorImpl->RegisterHandler(eventHandler="<<eventHandler<<", mask="<<mask<<") iRet="<<iRet<<endl;

    if( -1 == iRet )
    {
        eventHandler->SetReactor( oldReactor );
    }
    return iRet;
}
int Reactor::RegisterHandler( HANDLE ioHandle, EventHandler *eventHandler, ReactorMask mask )
{
    Reactor *oldReactor = eventHandler->GetReactor();

    eventHandler->SetReactor( this );

    int iRet = this->m_pReactorImpl->RegisterHandler( ioHandle, eventHandler, mask );

    if( -1 == iRet )
    {
        eventHandler->SetReactor( oldReactor );
    }
    return iRet;
}

int Reactor::RemoveHandler( EventHandler *eventHandler, ReactorMask mask )
{
    return this->m_pReactorImpl->RemoveHandler( eventHandler, mask );
}
int Reactor::RemoveHandler( HANDLE handle, ReactorMask mask )
{
    return this->m_pReactorImpl->RemoveHandler( handle, mask );
}

int Reactor::SuspendHandler( EventHandler *eventHandler )
{
    return this->m_pReactorImpl->SuspendHandler( eventHandler );
}
int Reactor::SuspendHandler( HANDLE handle )
{
    return this->m_pReactorImpl->SuspendHandler( handle );
}

int Reactor::ResumeHandler( EventHandler *eventHandler )
{
    return this->m_pReactorImpl->ResumeHandler( eventHandler );
}
int Reactor::ResumeHandler( HANDLE handle )
{
    return this->m_pReactorImpl->ResumeHandler( handle );
}

long Reactor::ScheduleTimer( EventHandler *eventHandler
                            , const void *arg
                            , const TimeValue &tvDelay
                            , const TimeValue &tvInterval )
{
    Reactor *oldReactor = eventHandler->GetReactor();

    eventHandler->SetReactor( this );

    long lResult = this->GetImplementation()->ScheduleTimer( eventHandler, arg, tvDelay, tvInterval );
    if( -1 == lResult )
    {
        eventHandler->SetReactor( oldReactor );
    }
    return lResult;
}
int Reactor::ResetTimerInterval( long lTimerId, const TimeValue &interval )
{
    return this->GetImplementation()->ResetTimerInterval( lTimerId, interval );
}
int Reactor::CancelTimer( long lTimerId
                        , const void **pAct
                        , int iDontCallHandleClose )
{
    return this->GetImplementation()->CancelTimer( lTimerId, pAct, iDontCallHandleClose );
}
int Reactor::CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose )
{
    return this->GetImplementation()->CancelTimer( eventHandler, iDontCallHandleClose );
}

EventHandler* Reactor::FindHandler( HANDLE handle )
{
    return this->m_pReactorImpl->FindHandler( handle );
}

int Reactor::Handler( HANDLE handle, ReactorMask mask, EventHandler **eventHandler )
{
    return this->m_pReactorImpl->Handler( handle, mask, eventHandler );
}

size_t Reactor::GetSize( void ) const
{
    return this->m_pReactorImpl->GetSize();
}

bool Reactor::GetRestartFlag( void )
{
    return this->m_pReactorImpl->GetRestartFlag();
}

bool Reactor::SetRestartFlag( bool bRestart )
{
    return this->m_pReactorImpl->SetRestartFlag( bRestart );
}

int Reactor::MaskOps( EventHandler *eventHandler, ReactorMask mask, int iOps )
{
    return this->m_pReactorImpl->MaskOps( eventHandler, mask, iOps );
}
int Reactor::MaskOps( HANDLE handle, ReactorMask mask, int iOps )
{
    return this->m_pReactorImpl->MaskOps( handle, mask, iOps );
}

ReactorImpl* Reactor::GetImplementation( void ) const
{
    return this->m_pReactorImpl;
}

void Reactor::SetImplemetation( ReactorImpl *impl )
{
    this->m_pReactorImpl = impl;
}

int Reactor::Notify( EventHandler *eventHandler
                , ReactorMask masks
                , TimeValue *timeout )
{
    if( eventHandler != NULL && NULL == eventHandler->GetReactor() )
    {
        eventHandler->SetReactor( this );
    }
    return this->m_pReactorImpl->Notify( eventHandler, masks, timeout );
}


void Reactor::WakeupAllThreads( void )
{
    this->m_pReactorImpl->WakeupAllThreads();
}


