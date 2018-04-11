#ifndef __TIME_QUEUE_T_H__
#define __TIME_QUEUE_T_H__

#include "Public.h"
#include "TimeValue.h"
#include "CopyDisabled.h"
#include "TimePolicy.h"
#include "FreeList.h"
#include "Lock.h"
#include "FunctorT.h"

template<class TYPE>
class TimerNodeDispatchInfoT
{
public:
    TYPE        m_Node;
    const void  *m_pAct;             //asynchronous completion token
    int         m_ReCurringTimer;   // Flag to check if the timer is recurring.
};

template<class TYPE>
class TimerNodeT
{
public:
    TimerNodeT( void );
    ~TimerNodeT( void );

    typedef TimerNodeDispatchInfoT<TYPE> DispatchInfo;

    /// Singly linked list
    void Init( const TYPE &node
              , const void *pAct
              , const TimeValue &timerValue
              , const TimeValue &timerInterval
              , TimerNodeT<TYPE> *pNext
              , long timerId )
    {
        this->m_Node = node;
        this->m_pAct = pAct;
        this->m_TimerValue = timerValue;
        this->m_TimerInterval = timerInterval;
        this->m_pNext = pNext;
        this->m_TimerId = timerId;
    }
    
    /// Doubly linked list version
    void Init( const TYPE &node
              , const void *pAct
              , const TimeValue &timerValue
              , const TimeValue &timerInterval
              , TimerNodeT<TYPE> *pPrevious
              , TimerNodeT<TYPE> *pNext
              , long timerId )
    {
        this->m_Node = node;
        this->m_pAct = pAct;
        this->m_TimerValue = timerValue;
        this->m_TimerInterval = timerInterval;
        this->m_pPrevious = pPrevious;
        this->m_pNext = pNext;
        this->m_TimerId = timerId;
    }


    TYPE& GetNode( void ) { return this->m_Node; }
    void SetNode( TYPE &node ) { this->m_Node = node; }

    const void* GetAct( void ) { return this->m_pAct; }
    void SetAct( const void *act ) { this->m_pAct = act; }

    const TimeValue& GetTimerValue( void ) const { return this->m_TimerValue; }
    void SetTimerValue( const TimeValue &timerValue ) { this->m_TimerValue = timerValue; }

    const TimeValue& GetTimerInterval( void ) const { return this->m_TimerInterval; }
    void SetTimerInterval( const TimeValue &timerInterval ) { this->m_TimerInterval = timerInterval; }

    TimerNodeT<TYPE>* GetPrevious( void ) { return this->m_pPrevious; }
    void SetPrevious( TimerNodeT<TYPE> *previous ) { this->m_pPrevious = previous; }

    TimerNodeT<TYPE>* GetNext( void ) { return this->m_pNext; }
    void SetNext( TimerNodeT<TYPE> *next ) { this->m_pNext = next; }

    long GetTimerId( void ) { return this->m_TimerId; }
    void SetTimerId( long timerId ) { this->m_TimerId = timerId; }

    void GetDispatchInfo( DispatchInfo &info )
    {
        info.m_Node = this->m_Node;
        info.m_pAct = this->m_pAct;
        info.m_ReCurringTimer = this->m_TimerInterval > TimeValue::zero;
    }

private:
    TYPE                m_Node;
    const void          *m_pAct;
    TimeValue           m_TimerValue;
    TimeValue           m_TimerInterval;
    TimerNodeT<TYPE>    *m_pPrevious;
    TimerNodeT<TYPE>    *m_pNext;
    long                m_TimerId;
};

template<class TYPE>
TimerNodeT<TYPE>::TimerNodeT( void )
    : m_pAct( NULL ), m_pPrevious( NULL ), m_pNext( NULL ), m_TimerId( -1 )
{
}
template<class TYPE>
TimerNodeT<TYPE>::~TimerNodeT( void )
{
}

template<class TYPE>
class TimerQueueIteratorT
{
public:
    TimerQueueIteratorT( void ) {}
    ~TimerQueueIteratorT( void ) {}

    virtual void GetFirst( void ) = 0;
    virtual void GetNext( void ) = 0;
    virtual bool IsDone( void ) const = 0;
    virtual TimerNodeT<TYPE>* GetCurrentNode( void ) = 0;
};

template<class TYPE>
class AbstractTimerQueue
{
public:
    virtual ~AbstractTimerQueue( void ) {}
    virtual bool IsEmpty( void ) const = 0;
    virtual const TimeValue& GetEarliestTime( void ) const = 0;
    virtual long Schedule( const TYPE &node
                            , const void *act
                            , const TimeValue &futureTime
                            , const TimeValue &interval = TimeValue::zero ) = 0;
    virtual TimeValue *CalculateTimeout( TimeValue *maxWaitTime, TimeValue *resTimeout ) = 0;    
    virtual int Expire( const TimeValue & ) = 0;
    virtual int ExpireSingle( CommandBase &preDispatchCmd ) = 0;
    virtual int ResetInterval( long timerId, const TimeValue &interval ) = 0;
    virtual int Cancel( const TYPE &node, int dontCallHandleClose = 1 ) = 0;
    virtual int Cancel( long timerId, const void **act = NULL, int dontCallHandleClose = 1 ) = 0;
    virtual int Close( void ) = 0;
    virtual TimeValue GetTimeOfDay( void ) = 0;
    virtual TimeValue GetCurrentTime() = 0;

    typedef TimerQueueIteratorT<TYPE> ITERATOR;

    virtual ITERATOR &GetIterator( void ) = 0;
    virtual TimerNodeT<TYPE>* GetFirstNode( void ) = 0;
    virtual TimerNodeT<TYPE>* RemoveFirstNode( void ) = 0;
};

class EventHandler;
typedef AbstractTimerQueue<EventHandler*> TimerQueue;

template<class TYPE, class FUNCTOR>
class TimerQueueCallbackBase: public AbstractTimerQueue<TYPE>, private CopyDisabled
{
public:
    explicit TimerQueueCallbackBase( FUNCTOR *callbackFunc = NULL );
    virtual ~TimerQueueCallbackBase( void );

    FUNCTOR& GetCallbackFunctor( void ) { return *this->m_pCallbackFunctor; }

protected:
    FUNCTOR     *m_pCallbackFunctor;
    bool const  m_bDelCallbackFunctor;
};

template<class TYPE, class FUNCTOR>
TimerQueueCallbackBase<TYPE, FUNCTOR>::TimerQueueCallbackBase( FUNCTOR *callbackFunc )
    : AbstractTimerQueue<TYPE>(), CopyDisabled(), m_pCallbackFunctor( callbackFunc ), m_bDelCallbackFunctor( NULL == callbackFunc )
{
    if( callbackFunc != NULL )
    {
        return;
    }
    NEW( this->m_pCallbackFunctor, FUNCTOR );
}

template<class TYPE, class FUNCTOR>
TimerQueueCallbackBase<TYPE, FUNCTOR>::~TimerQueueCallbackBase( void )
{
    if( this->m_bDelCallbackFunctor )
    {
        delete this->m_pCallbackFunctor;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY = DefaultTimePolicy>
class TimerQueueT: public TimerQueueCallbackBase<TYPE, FUNCTOR>
{
public:
    TimerQueueT( FUNCTOR *callBack = NULL
                , FreeList<TimerNodeT<TYPE> > *list = NULL
                , TIME_POLICY const &timePolicy = TIME_POLICY() );

    virtual ~TimerQueueT( void );
    virtual long Schedule( const TYPE &node
                            , const void *act
                            , const TimeValue &futureTime
                            , const TimeValue &interval = TimeValue::zero );
    virtual TimeValue *CalculateTimeout( TimeValue *maxWaitTime, TimeValue *resTimeout );
    virtual int Expire( const TimeValue &currTime );
    virtual int ExpireSingle( CommandBase &preDispatchCmd );
    virtual TimeValue GetTimeOfDay( void );
    TimeValue GetTimeOfDayStatic( void );
    virtual TimeValue GetCurrentTime();

    const TimeValue& GetTimerSkew( void ) const { return this->m_TimerSkew; }
    void SetTimerSkew( TimeValue &timerSkew ) { this->m_TimerSkew = timerSkew; }
    LOCK& GetLock( void ) { return this->m_Lock; }

    void PreInvoke( TimerNodeDispatchInfoT<TYPE> &info
                    , const TimeValue &currTime
                    , const void *&callBackAct );

    /// This method will call the timeout() on <functor>.
    void CallBack( TimerNodeDispatchInfoT<TYPE> &info
                    , const TimeValue &currTime);

    void PostInvoke( TimerNodeDispatchInfoT<TYPE> &info
                    , const TimeValue &currTime
                    , const void *callBackAct );

protected:
    virtual TimerNodeT<TYPE>* AllocNode( void );
    virtual void FreeNode( TimerNodeT<TYPE> *node );
    
    virtual long ScheduleAux( const TYPE &node
                            , const void *act
                            , const TimeValue &futureTime
                            , const TimeValue &interval ) = 0;

    virtual int DispatchInfoAux( const TimeValue &currTimer, TimerNodeDispatchInfoT<TYPE> &info );
    virtual void ReSchedule( TimerNodeT<TYPE> * ) = 0;
    
    void RecomputeNextAbsIntervalTime( TimerNodeT<TYPE>* expired, const TimeValue &currTime );

protected:
    LOCK                        m_Lock;
    FreeList<TimerNodeT<TYPE> > *m_pList;
    TIME_POLICY                 m_TimePolicy;
    bool const                  m_bDelList;
private:
    TimeValue                   m_TimeOut;
    TimeValue                   m_TimerSkew;
};

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::TimerQueueT( FUNCTOR *callBack
                                                            , FreeList<TimerNodeT<TYPE> > *list
                                                            , TIME_POLICY const &timePolicy )
    : TimerQueueCallbackBase<TYPE, FUNCTOR>( callBack )
    , m_TimePolicy( timePolicy )
    , m_bDelList( NULL == list )
{
    if( list != NULL )
    {
        this->m_pList = list;
    }
    else
    {
        NEW( this->m_pList, (LockedFreeList<TimerNodeT<TYPE>,NullMutex>) );
    }
}
template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::~TimerQueueT( void )
{
    if( this->m_bDelList )
    {
        delete this->m_pList;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimeValue* TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::CalculateTimeout( TimeValue *maxWaitTime, TimeValue *resTimeout )
{
    if( NULL == resTimeout )
    {
        return NULL;
    }

    GUARD_RETURN( LOCK, ace_mon, this->m_Lock, maxWaitTime );

    if( this->IsEmpty() )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::CalculateTimeout IsEmpty!"<<endl;
        // Nothing on the Timer_Queue, so use whatever the caller gave us.
        if(maxWaitTime)
        {
            *resTimeout = *maxWaitTime;
        }
        else
        {
            resTimeout = NULL;
        }
    }
    else
    {
        TimeValue curTime = this->GetTimeOfDayStatic();

        if( this->GetEarliestTime() > curTime )
        {
            // The earliest item on the Timer_Queue is still in the
            // future.  Therefore, use the smaller of (1) caller's wait
            // time or (2) the delta time between now and the earliest
            // time on the Timer_Queue.

            *resTimeout = this->GetEarliestTime() - curTime;
            if( !( NULL == maxWaitTime || *maxWaitTime > *resTimeout) )
            {
                *resTimeout = *maxWaitTime;
            }
        }
        else
        {
            // The earliest item on the Timer_Queue is now in the past.
            // Therefore, we've got to "poll" the Reactor, i.e., it must
            // just check the descriptors and then dispatch timers, etc.
            resTimeout = NULL;
        }
    }
    return resTimeout;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
long TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Schedule( const TYPE &node
                                                            , const void *act
                                                            , const TimeValue &futureTime
                                                            , const TimeValue &interval )
{
    long lRet = this->ScheduleAux( node, act, futureTime, interval );
    if( -1 == lRet )
    {
        return -1;
    }

    this->GetCallbackFunctor().Registration( *this, node, act );

    return lRet;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Expire( const TimeValue &currTime )
{
    GUARD_RETURN( LOCK, guard, this->m_Lock, -1 );

    if( this->IsEmpty() )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::Expire TimerQueue IsEmpty!"<<endl;
        return 0;
    }

    int iNumOfTimersExpired = 0;
    int iRes = 0;
    
    TimerNodeDispatchInfoT<TYPE> dispatchInfo;
    
    while( (iRes = this->DispatchInfoAux(currTime, dispatchInfo)) != 0 )
    {
        ReverseLock<LOCK> revLock( this->m_Lock );
        GUARD_RETURN( ReverseLock<LOCK>, gurad, revLock, -1 );

        const void *callBackAct = 0;

        this->PreInvoke( dispatchInfo, currTime, callBackAct );

        this->CallBack( dispatchInfo, currTime );

        this->PostInvoke( dispatchInfo, currTime, callBackAct );

        ++iNumOfTimersExpired;
    }

    return iNumOfTimersExpired;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ExpireSingle( CommandBase &preDispatchCmd )
{
    TimerNodeDispatchInfoT<TYPE> info;
    TimeValue currTime;
    {
        // Create a scope for the lock ...
        GUARD_RETURN( LOCK, guard, this->m_Lock, -1 );

        if( this->IsEmpty() )
        {
            cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle TimerQueue IsEmpty!"<<endl;
            return 0;
        }

        // Get the current time
        currTime = this->GetTimeOfDayStatic() + this->GetTimerSkew();
        cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle currTime="<<currTime.GetSecond()<<endl;

        // Look for a node in the timer queue whose timer <= the present time.
        if( 0 == this->DispatchInfoAux(currTime, info) )
        {
            return 0;
        }
    }
    // We do not need the lock anymore, all these operations take place
    // with local variables.
    const void *callBackAct = NULL;

    // Preinvoke (handles refcount if needed, etc.)
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle this->PreInvoke(info, currTime, callBackAct) Begin!"<<endl;
    this->PreInvoke(info, currTime, callBackAct);

    // Release the token before expiration upcall.
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle preDispatchCmd.Execute Begin!"<<endl;
    preDispatchCmd.Execute();

    // call the functor
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle this->CallBack( info, currTime ) Begin!"<<endl;
    this->CallBack( info, currTime );

    // Postinvoke (undo refcount if needed, etc.)
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::ExpireSingle this->PostInvoke( info, currTime, callBackAct ) Begin!"<<endl;
    this->PostInvoke( info, currTime, callBackAct );

    // We have dispatched a timer
    return 1;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimeValue TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetTimeOfDayStatic( void )
{
    return this->m_TimePolicy();
}
template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimeValue TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetTimeOfDay( void )
{
    return this->GetTimeOfDayStatic();
}
template <class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimeValue TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetCurrentTime()
{
    TimeValue tv = this->GetTimeOfDayStatic();
    tv += this->GetTimerSkew();
    return tv;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::PreInvoke( TimerNodeDispatchInfoT<TYPE> &info
                , const TimeValue &currTime
                , const void *&callBackAct )
{
    this->GetCallbackFunctor().PreInvoke( *this, info.m_Node, info.m_pAct, info.m_ReCurringTimer, currTime, callBackAct );
}

/// This method will call the timeout() on <functor>.
template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::CallBack( TimerNodeDispatchInfoT<TYPE> &info
                , const TimeValue &currTime)
{
    this->GetCallbackFunctor().TimeOut( *this, info.m_Node, info.m_pAct, info.m_ReCurringTimer, currTime );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::PostInvoke( TimerNodeDispatchInfoT<TYPE> &info
                , const TimeValue &currTime
                , const void *callBackAct )
{
    this->GetCallbackFunctor().PostInvoke( *this, info.m_Node, info.m_pAct, info.m_ReCurringTimer, currTime, callBackAct );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE>* TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::AllocNode(void)
{
    return this->m_pList->Remove();
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::FreeNode(TimerNodeT<TYPE> *node)
{
    this->m_pList->Add( node );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::DispatchInfoAux( const TimeValue &currTime, TimerNodeDispatchInfoT<TYPE> &info )
{
    if( this->IsEmpty() )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux TimerQueue IsEmpty!"<<endl;
        return 0;
    }

    TimerNodeT<TYPE> *expired = NULL;

    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux this->GetEarliestTime()="<<this->GetEarliestTime().GetSecond()<<endl;
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux currTime="<<currTime.GetSecond()<<endl;
    if( this->GetEarliestTime() <= currTime )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux this->RemoveFirstNode()"<<endl;
        expired = this->RemoveFirstNode();

        // Get the dispatch info
        expired->GetDispatchInfo( info );

        // Check if this is an interval timer.
        if( expired->GetTimerInterval() > TimeValue::zero )
        {
            // Make sure that we skip past values that have already "expired".
            cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux this->RecomputeNextAbsIntervalTime(expired, currTime)"<<endl;
            this->RecomputeNextAbsIntervalTime(expired, currTime);

            // Since this is an interval timer, we need to reschedule
            // it.
            cout<<__FILE__<<"_"<<__LINE__<<"__TimerQueueT<...>::DispatchInfoAux this->ReSchedule( expired )"<<endl;
            this->ReSchedule( expired );
        }
        else
        {
            // Call the factory method to free up the node.
            this->FreeNode( expired );
        }
        return 1;
    }

    return 0;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::RecomputeNextAbsIntervalTime( TimerNodeT<TYPE>* expired, const TimeValue &currTime )
{
    if( expired->GetTimerValue() <= currTime )
    {
        uint64 iIntervalUSec;
        expired->GetTimerInterval().ToSuSecond( iIntervalUSec );

        
        TimeValue oldDiff = currTime - expired->GetTimerValue();
        uint64 oldDiffUsec;
        oldDiff.ToSuSecond(oldDiffUsec);

        // Compute the delta time in the future when the timer
        // should fire as if it had advanced incrementally.  The
        // modulo arithmetic accomodates the likely case that
        // the current time doesn't fall precisely on a timer
        // firing interval.
        uint64 newTimerUSec = iIntervalUSec - ( oldDiffUsec % iIntervalUSec );

        // Compute the absolute time in the future when this
        // interval timer should expire.
        TimeValue newTimerValue( currTime.GetSecond() + static_cast<time_t>(newTimerUSec / ONE_SECOND_IN_USECS)
                                , currTime.GetSuSecond()+ static_cast<suseconds_t>(newTimerUSec % ONE_SECOND_IN_USECS) );

        expired->SetTimerValue( newTimerValue );
    }
}

#endif

