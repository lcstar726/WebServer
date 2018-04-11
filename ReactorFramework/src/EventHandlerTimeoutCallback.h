#ifndef __EVENT_HANDLER_TIMEOUT_CALLBACK_H__
#define __EVENT_HANDLER_TIMEOUT_CALLBACK_H__

#include "Public.h"
#include "CopyDisabled.h"
#include "TimerQueueT.h"
#include "EventHandler.h"

class EventHandlerTimeoutCallback: private CopyDisabled
{
public:
    EventHandlerTimeoutCallback( void );
    ~EventHandlerTimeoutCallback( void );

    int Registration( TimerQueue &timerQueue, EventHandler *handler, const void *arg );

    int PreInvoke( TimerQueue &timerQueue
                , EventHandler *handler
                , const void *arg
                , int iReCurringTimer
                , const TimeValue &currTime
                , const void *&callbackAct );

    int PostInvoke( TimerQueue &timerQueue
                , EventHandler *handler
                , const void *arg
                , int iReCurringTimer
                , const TimeValue &currTime
                , const void *&callbackAct );

    int TimeOut( TimerQueue &timerQueue
                , EventHandler *handler
                , const void *arg
                , int iReCurringTimer
                , const TimeValue &currTime );

    /// This method is called when a handler is cancelled
    int CancelNode( TimerQueue &timerQueue
                    , EventHandler *handler
                    , int iDontCall
                    , int &iRequiresReferenceCounting );

    /// This method is called when a timer is cancelled
    int CancelTimer( TimerQueue &timerQueue
                    , EventHandler *handler
                    , int iDontCall
                    , int iRequiresReferenceCounting );

    int Deletion( TimerQueue &timerQueue, EventHandler *handler, const void *arg );

private:
    int m_iRequiresReferenceCounting;
};

#endif

