#include "EventHandlerTimeoutCallback.h"
#include "Reactor.h"

EventHandlerTimeoutCallback::EventHandlerTimeoutCallback( void )
    : m_iRequiresReferenceCounting( 0 )
{
}

EventHandlerTimeoutCallback::~EventHandlerTimeoutCallback( void )
{
}

int EventHandlerTimeoutCallback::Registration( TimerQueue &timerQueue, EventHandler *eventHandler, const void *arg )
{
    eventHandler->AddReference();
    return 0;
}

int EventHandlerTimeoutCallback::PreInvoke( TimerQueue &timerQueue
        , EventHandler *eventHandler
        , const void *arg
        , int iReCurringTimer
        , const TimeValue &currTime
        , const void *&callbackAct )
{
    bool const requires_reference_counting = eventHandler->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

    if (requires_reference_counting)
    {
        eventHandler->AddReference();

        callbackAct = &this->m_iRequiresReferenceCounting;
    }

    return 0;
}

int EventHandlerTimeoutCallback::PostInvoke( TimerQueue &timerQueue
        , EventHandler *eventHandler
        , const void *arg
        , int iReCurringTimer
        , const TimeValue &currTime
        , const void *&callbackAct )
{
    if( callbackAct == &this->m_iRequiresReferenceCounting )
    {
        eventHandler->RemoveReference();
    }
    
    return 0;
}

int EventHandlerTimeoutCallback::TimeOut( TimerQueue &timerQueue
                                        , EventHandler *eventHandler
                                        , const void *pAct
                                        , int iReCurringTimer
                                        , const TimeValue &currTime )
{
    int iRequiresReferenceCounting = 0;

    if( !iReCurringTimer )
    {
        iRequiresReferenceCounting =
            eventHandler->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;
    }

    // Upcall to the <eventHandler>s handle_timeout method.
    cout<<__FILE__<<"_"<<__LINE__<<"__EventHandlerTimeoutCallback::TimeOut eventHandler->HandleTimeout(currTime, pAct) Begin!"<<endl;
    if( eventHandler->HandleTimeout(currTime, pAct) == -1)
    {
        if( eventHandler->GetReactorTimerInterface() != NULL )
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__EventHandlerTimeoutCallback::TimeOut eventHandler->GetReactorTimerInterface()->CancelTimer( eventHandler, 0 ) Begin!"<<endl;
            eventHandler->GetReactorTimerInterface()->CancelTimer( eventHandler, 0 );
        }
        else
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__EventHandlerTimeoutCallback::TimeOut timerQueue.Cancel( eventHandler, 0 ) Begin!"<<endl;
            timerQueue.Cancel( eventHandler, 0 );  // 0 means "call handle_close()".
        }
    }

    if( !iReCurringTimer && iRequiresReferenceCounting )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__EventHandlerTimeoutCallback::TimeOut eventHandler->RemoveReference() Begin!"<<endl;
        eventHandler->RemoveReference();
    }

    return 0;
}

/// This method is called when a eventHandler is cancelled
int EventHandlerTimeoutCallback::CancelNode( TimerQueue &timerQueue
            , EventHandler *eventHandler
            , int iDontCall
            , int &iRequiresReferenceCounting )
{
    iRequiresReferenceCounting =
        eventHandler->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

    // Upcall to the <handler>s handle_close method
    if( iDontCall == 0 )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__EventHandlerTimeoutCallback::CancelNode eventHandler->HandleClose( INVALID_HANDLE, EventHandler::TIMER_MASK ) Begin!"<<endl;
        eventHandler->HandleClose( INVALID_HANDLE, EventHandler::TIMER_MASK );
    }

    return 0;

}

/// This method is called when a timer is cancelled
int EventHandlerTimeoutCallback::CancelTimer( TimerQueue &timerQueue
                                            , EventHandler *eventHandler
                                            , int iDontCall
                                            , int iRequiresReferenceCounting )
{
    if( iRequiresReferenceCounting != 0 )
    {
        eventHandler->RemoveReference();
    }
    
    return 0;
}

int EventHandlerTimeoutCallback::Deletion( TimerQueue &timerQueue, EventHandler *eventHandler, const void *arg )
{
    int iRequiresReferenceCounting = 0;

    this->CancelNode( timerQueue, eventHandler, NULL, iRequiresReferenceCounting );

    this->CancelTimer( timerQueue, eventHandler, NULL, iRequiresReferenceCounting );

    return 0;
}

