#ifndef __REACTOR_H__
#define __REACTOR_H__

#include "Public.h"
#include "TimeValue.h"
#include "TimerHeapT.h"

class EventHandler;
class ReactorImpl;

class ReactorTimerInterface
{
public:
    virtual ~ReactorTimerInterface( void );

    virtual long ScheduleTimer( EventHandler *eventHandler
                                , const void *pAct
                                , const TimeValue &delay
                                , const TimeValue &interval = TimeValue::zero ) = 0;

    virtual int ResetTimerInterval( long lTimerId, const TimeValue &interval ) = 0;

    virtual int CancelTimer( long lTimerId
                            , const void **pAct = NULL
                            , int iDontCallHandleClose = 1 ) = 0;

    virtual int CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose = 1 ) = 0;
};

class Reactor: public ReactorTimerInterface
{
public:
    enum
    {
        GET_MASK = 1
        , SET_MASK = 2
        , ADD_MASK = 3
        , CLR_MASK = 4
    };

    Reactor( ReactorImpl *implementation = NULL, bool bDelImplementation = false );
    virtual ~Reactor( void );

    typedef int(*REACTOR_EVENT_HOOK)( Reactor * );

    static Reactor *GetInstance( void );

    int RunReactorEventLoop( REACTOR_EVENT_HOOK = NULL );

    int EndReactorEventLoop( void );

    int ReactorEventLoopDone( void );

    int Open( size_t iMaxHandlersNumber, bool bRestart = false );

    TimerQueue* GetTimerQueue( void );
    int SetTimerQueue( TimerQueue *timerQueue );

    int Close( void );

    int HandleEvents( TimeValue *maxWaitTime = NULL );

    int RegisterHandler( EventHandler *eventHandler, ReactorMask mask );
    int RegisterHandler( HANDLE ioHandle, EventHandler *eventHandler, ReactorMask mask );

    int RemoveHandler( EventHandler *eventHandler, ReactorMask mask );
    int RemoveHandler( HANDLE handle, ReactorMask mask );

    int SuspendHandler( EventHandler *eventHandler );
    int SuspendHandler( HANDLE handle );

    int ResumeHandler( EventHandler *eventHandler );
    int ResumeHandler( HANDLE handle );

    virtual long ScheduleTimer( EventHandler *eventHandler
                            , const void *pAct
                            , const TimeValue &tvDelay
                            , const TimeValue &tvInterval = TimeValue::zero );
    virtual int ResetTimerInterval( long lTimerId, const TimeValue &interval );
    virtual int CancelTimer( long lTimerId
                            , const void **pAct = NULL
                            , int iDontCallHandleClose = 1 );
    virtual int CancelTimer( EventHandler *eventHandler, int iDontCallHandleClose = 1 );

    EventHandler* FindHandler( HANDLE handle );
    int Handler( HANDLE handle, ReactorMask mask, EventHandler **eventHandler = NULL );

    size_t GetSize( void ) const;

    bool GetRestartFlag( void );
    bool SetRestartFlag( bool bRestart );

    int MaskOps( EventHandler *eventHandler, ReactorMask mask, int iOps );
    int MaskOps( HANDLE handle, ReactorMask mask, int iOps );

    ReactorImpl* GetImplementation( void ) const;

    int Notify( EventHandler *eventHandler = NULL
                , ReactorMask masks = EventHandler::EXCEPT_MASK
                , TimeValue *timeout = 0 );

    void WakeupAllThreads( void );

protected:
    void SetImplemetation( ReactorImpl *impl );

    ReactorImpl     *m_pReactorImpl;
    bool            m_bDelImplemetation;
    
    static Reactor  *m_pReactor;
    static bool     m_bDelReactor;
};

#endif

