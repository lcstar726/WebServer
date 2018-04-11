#ifndef __REACTOR_IMPL_H__
#define __REACTOR_IMPL_H__

#include "Public.h"
#include "TimeValue.h"
#include "TimerQueueT.h"
#include "EventHandler.h"
#include "SynchTraits.h"
#include "Token.h"

class ReactorImpl
{
public:
    virtual ~ReactorImpl( void );

    virtual int Open( size_t size
                    , bool bRestart = false
                    , TimerQueue *pTimerQueue = NULL ) = 0;

    virtual int Close( void ) = 0;

    virtual int HandleEvents( TimeValue *maxWaitTime = NULL ) = 0;
    virtual int AlertTableHandleEvents( TimeValue *maxWaitTime = NULL ) = 0;

    virtual TimerQueue* GetTimerQueue( void ) = 0;
    virtual int SetTimerQueue( TimerQueue *timerQueue ) = 0;

    virtual int GetDeactivated( void ) = 0;
    virtual void SetDeactivate( int iDoStop ) = 0;

    virtual int RegisterHandler( EventHandler *eventHandler, ReactorMask mask ) = 0;
    virtual int RegisterHandler( HANDLE IOHandle, EventHandler *eventHandler, ReactorMask mask ) = 0;

    virtual int RemoveHandler( EventHandler *eventHandler, ReactorMask mask ) = 0;
    virtual int RemoveHandler( HANDLE handle, ReactorMask mask ) = 0;

    virtual int SuspendHandler( EventHandler *eventHandler ) = 0;
    virtual int SuspendHandler( HANDLE handle ) = 0;
    virtual int ResumeHandler( EventHandler *eventHandler ) = 0;
    virtual int ResumeHandler( HANDLE handle ) = 0;

    virtual long ScheduleTimer( EventHandler *eventHandler
                            , const void *arg
                            , const TimeValue &tvDelay
                            , const TimeValue &tvInterval ) = 0;
    virtual int ResetTimerInterval( long lTimerId, const TimeValue &interval ) = 0;

    virtual int CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose = 1 ) = 0;
    virtual int CancelTimer( long lTimerId
                            , const void **pAct = NULL
                            , int iDontCallHandleClose = 1 ) = 0;

    //virtual int ScheduleWakeup( EventHandler *eventHandler, ReactorMask masksToBeAdded ) = 0;
    //virtual int ScheduleWakeup( HANDLE handle, ReactorMask masksToBeAdded ) = 0;
    //virtual int CancelWakeup( EventHandler *eventHandler, ReactorMask masksToBeCleared ) = 0;
    //virtual int CancelWakeup( HANDLE handle, ReactorMask maskToBeCleared ) = 0;

    virtual EventHandler* FindHandler( HANDLE handle ) = 0;
    virtual int Handler( HANDLE handle, ReactorMask mask, EventHandler **eventHandler = NULL ) = 0;

    virtual bool IsInitialized( void ) = 0;

    virtual size_t GetSize( void ) const = 0;

    virtual bool GetRestartFlag( void ) = 0;
    virtual bool SetRestartFlag( bool bRestart ) = 0;

    virtual int MaskOps( EventHandler *eventHandler, ReactorMask mask, int iOps ) = 0;
    virtual int MaskOps( HANDLE handle, ReactorMask mask, int iOps ) = 0;

    virtual int Notify( EventHandler *eventHandler = NULL
                    , ReactorMask masks = EventHandler::EXCEPT_MASK
                    , TimeValue *timeout = 0 ) = 0;

    virtual void WakeupAllThreads( void ) = 0;
};

template< class TokenType>
class ReactorTokenT: public TokenType
{
public:
    ReactorTokenT( ReactorImpl &impl, int iQueueType = TokenType::FIFO );
    ReactorTokenT( int iQueueType = TokenType::FIFO );
    virtual ~ReactorTokenT( void );

    virtual void SleepHook( void );

    ReactorImpl& GetReactorImpl( void );
    void SetReactorImpl( ReactorImpl &impl );

private:
    ReactorImpl     *m_pReactorImpl;
};

template<class TokenType>
ReactorTokenT<TokenType>::ReactorTokenT( ReactorImpl &impl, int iQueueType )
    : m_pReactorImpl( impl )
{
    this->SetQueueingStrategy( iQueueType );
}
template<class TokenType>
ReactorTokenT<TokenType>::ReactorTokenT( int iQueueType )
{
    this->SetQueueingStrategy( iQueueType );
}
template<class TokenType>
ReactorTokenT<TokenType>::~ReactorTokenT( void )
{
}
template<class TokenType>
void ReactorTokenT<TokenType>::SleepHook( void )
{
    TimeValue ping = TimeValue::zero;
    if( -1 == this->m_pReactorImpl->Notify( NULL, EventHandler::EXCEPT_MASK, &ping) )
    {
        if( ETIME == errno )
        {
            errno = 0;
        }
        else
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__ReactorTokenT<TokenType>::SleepHook error!"<<endl;
        }
    }
}

template<class TokenType>
ReactorImpl& ReactorTokenT<TokenType>::GetReactorImpl( void )
{
    return *this->m_pReactorImpl;
}
template<class TokenType>
void ReactorTokenT<TokenType>::SetReactorImpl( ReactorImpl &impl )
{
    this->m_pReactorImpl = &impl;
}

typedef Token DevPollToken;
typedef ReactorTokenT<DevPollToken> DevPollReactorToken;

class DevPollReactorImpl: public ReactorImpl
{
private:
    struct EventTuple
    {
        EventTuple( EventHandler *eh = NULL
                    , ReactorMask mask = EventHandler::NULL_MASK
                    , bool bIsSuspended = false
                    , bool bIsControlled = false )
            : m_pEvtHandler( eh ), m_Mask( mask ), m_bIsSuspended( bIsSuspended ), m_bIsControlled( bIsControlled )
        {
        }
        
        EventHandler    *m_pEvtHandler;
        ReactorMask     m_Mask;
        bool            m_bIsSuspended;
        bool            m_bIsControlled;
    };

    class HandlerRepository
    {
    public:
        HandlerRepository( void );
        int Open( size_t size );
        EventTuple* Find( HANDLE handle );

        int Bind( HANDLE handle, EventHandler *eventHandler, ReactorMask mask );
        int UnBind( HANDLE handle, bool bDecRefcnt = true );
        int UnBindAll( void );
        int Close( void );

        bool HandleIsInvalid( HANDLE handle ) const;
        bool HandleInRange( HANDLE handle ) const;

        size_t GetSize( void ) const;
        size_t GetMaxSize( void ) const;

    private:
        size_t      m_Size;
        size_t      m_MaxSize;
        EventTuple  *m_pEvtTupHandlers;
    };

public:
    DevPollReactorImpl( TimerQueue *pTimerQueue = NULL );
    DevPollReactorImpl( size_t size, bool bRestart = false, TimerQueue *pTimerQueue = NULL );
    virtual ~DevPollReactorImpl( void );

    virtual int Open( size_t size
                    , bool bRestart = false
                    , TimerQueue *pTimerQueue = NULL );
    virtual int Close( void );

    virtual TimerQueue* GetTimerQueue( void );
    virtual int SetTimerQueue( TimerQueue *timerQueue );

    virtual int HandleEvents( TimeValue *maxWaitTime = NULL );
    virtual int AlertTableHandleEvents( TimeValue *maxWaitTime = NULL );

    virtual int GetDeactivated( void );
    virtual void SetDeactivate( int iDoStop );

    virtual int RegisterHandler( EventHandler * eventHandler,ReactorMask mask );
    virtual int RegisterHandler( HANDLE ioHandle, EventHandler * eventHandler,ReactorMask mask );
    virtual int RemoveHandler( EventHandler *eventHandler, ReactorMask mask );
    virtual int RemoveHandler( HANDLE handle, ReactorMask mask );

    virtual int SuspendHandler( EventHandler *eventHandler );
    virtual int SuspendHandler( HANDLE handle );
    virtual int ResumeHandler( EventHandler *eventHandler );
    virtual int ResumeHandler( HANDLE handle );

    virtual long ScheduleTimer( EventHandler *eventHandler
                            , const void *arg
                            , const TimeValue &tvDelay
                            , const TimeValue &tvInterval = TimeValue::zero );
    virtual int ResetTimerInterval( long lTimerId, const TimeValue &interval );

    virtual int CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose = 1 );
    virtual int CancelTimer( long lTimerId
                            , const void **pAct = NULL
                            , int iDontCallHandleClose = 1 );

    //virtual int ScheduleWakeup( EventHandler *eventHandler, ReactorMask maskToBeAdded );
    //virtual int ScheduleWakeup( HANDLE handle, ReactorMask maskToBeAdded );
    //virtual int CancelWakeup( EventHandler *eventHandler, ReactorMask maskToBeAdded );
    //virtual int CancelWakeup( HANDLE handle, ReactorMask maskToBeAdded );

    virtual EventHandler* FindHandler( HANDLE handle );
    virtual int Handler( HANDLE handle, ReactorMask mask, EventHandler **eventHandler = NULL );

    virtual bool IsInitialized( void );

    virtual size_t GetSize( void ) const;

    virtual bool GetRestartFlag( void );
    virtual bool SetRestartFlag( bool bRestart );

    virtual int MaskOps( EventHandler *eventHandler, ReactorMask mask, int iOps );
    virtual int MaskOps( HANDLE handle, ReactorMask mask, int iOps );

    int Notify( EventHandler *eventHandler = NULL
                , ReactorMask masks = EventHandler::EXCEPT_MASK
                , TimeValue *timeout = 0 );

    virtual void WakeupAllThreads( void );

protected:
    class TokenGuard
    {
    private:
        TokenGuard( void );
    public:
        TokenGuard( DevPollReactorToken &token );
        ~TokenGuard( void );

        void ReleaseToken( void );
        int IsOwner( void );
        //int AcquireQuietly( TimeValue *timeOut = NULL );
        int Acquire( TimeValue *timeOut = NULL );

    private:
        DevPollReactorToken &m_Token;
        int                 m_Owner;
    };

protected:
    int UpCall( EventHandler *eventHandler
                , int (EventHandler::*CallBack)(HANDLE)
                , HANDLE handle );

    int RegisterHandlerAux( HANDLE handle, EventHandler *eventHandler, ReactorMask mask );
    int ResumeHandlerAux( HANDLE handle );
    int RemoveHandlerAux( HANDLE handle
                        , ReactorMask mask
                        , SynchGuard<SYNCH_MUTEX> &hdrRepoGuard
                        , EventHandler *eventHandler = NULL );
    int SuspendHandlerAux( HANDLE handle );

    short ReactorMaskToPollEvent( ReactorMask mask );
    int MaskOpsAux( HANDLE handle, ReactorMask mask, int iOps );

    int WorkPendingAux( TimeValue *maxWaitTime );

    int HandleEventsAux( TimeValue *maxWaitTime, TokenGuard &guard );
    int DispatchTimerEvent( TokenGuard &guard );
    int DispatchIOEvent( TokenGuard &guard );
    int Dispatch( TokenGuard &guard );

protected:
    bool                m_bIsInitialized;
    HANDLE              m_ePollFd;
    struct  epoll_event m_ePollEvent;
    DevPollReactorToken m_Token;
    sig_atomic_t        m_Deactivated;
    HandlerRepository   m_HandlerRepository;
    
    TimerQueue          *m_pTimerQueue;
    bool                m_bDelTimerQueue;

    SYNCH_MUTEX         m_HdlrRepoLock;
    bool                m_bRestart;
};

#endif

