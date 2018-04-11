#ifndef __TIMER_HEAP_H__
#define __TIMER_HEAP_H__

#include "Public.h"
#include "TimerQueueT.h"
#include "FreeList.h"
#include "TimePolicy.h"
#include "EventHandlerTimeoutCallback.h"

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
class TimerHeapT;

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
class TimerHeapIteratorT: public TimerQueueIteratorT<TYPE>
{
public:
    typedef TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY> Heap;

    TimerHeapIteratorT( Heap &heap );
    virtual ~TimerHeapIteratorT( void );

    virtual void GetFirst( void );
    virtual void GetNext( void );
    virtual bool IsDone( void ) const;
    virtual TimerNodeT<TYPE>* GetCurrentNode( void );

protected:
    Heap&   m_TimerHeap;
    size_t  m_Position;
};

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY = DefaultTimePolicy>
class TimerHeapT: public TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY>
{
    friend class TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>;
public:
    typedef TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY> HeapIterator;
    typedef TimerQueueT<TYPE, FUNCTOR, LOCK, TIME_POLICY> BaseTimePolicy;

    TimerHeapT( size_t capacity
                , bool bPreAllocated = false
                , FUNCTOR *pCallbackFunctor = NULL
                , FreeList<TimerNodeT<TYPE> > *pFreeList = NULL
                , TIME_POLICY const &timePolicy = TIME_POLICY() );

    TimerHeapT( FUNCTOR *CallbackFunctor = NULL
                , FreeList<TimerNodeT<TYPE> > *pFreeList = NULL
                , TIME_POLICY const &timePolicy = TIME_POLICY() );
    virtual ~TimerHeapT( void );

    virtual bool IsEmpty( void) const;
    virtual const TimeValue &GetEarliestTime( void ) const;
    virtual int ResetInterval( long timerId, const TimeValue &interval );
    virtual int Cancel( const TYPE &node, int dontCallHandleClose = 1 );
    virtual int Cancel( long timerId, const void **pAct = 0, int dontCallHandleClose = 1 );
    virtual int Close( void);
    virtual TimerQueueIteratorT<TYPE>& GetIterator( void );
    virtual TimerNodeT<TYPE>* RemoveFirstNode( void );
    virtual TimerNodeT<TYPE>* GetFirstNode( void );

protected:
    virtual long ScheduleAux( const TYPE &node, const void *pAct, const TimeValue &futureTime, const TimeValue &interval );
    virtual void ReSchedule( TimerNodeT<TYPE> *expired );
    virtual TimerNodeT<TYPE> *AllocNode( void );
    virtual void FreeNode( TimerNodeT<TYPE> * );

private:
    TimerNodeT<TYPE> *Remove( size_t slot );
    void Insert( TimerNodeT<TYPE> *pNewNode );
    void GrowHeap( void );
    void ReHeapUp( TimerNodeT<TYPE> *pNewNode, size_t slot, size_t parent );
    void ReHeapDown( TimerNodeT<TYPE> *pMovedNode, size_t slot, size_t child );
    void Copy( size_t slot, TimerNodeT<TYPE> *pMovedNode );
    long GetTimerId( void );
    long PopFreeList( void );
    void PushFreeList( long old_id );

private:
    size_t              m_Capacity;
    size_t              m_Size;
    size_t              m_LimboSize;
    HeapIterator        *m_pIterator;
    TimerNodeT<TYPE>    **m_pHeap;
    ssize_t             *m_pTimerIds;
    size_t              m_CurrTimerIds;
    size_t              m_TimerIdsMinFree;
    TimerNodeT<EventHandler*>    *m_pPreAllocatedNodes;
    TimerNodeT<EventHandler*>    *m_pPreAllocatedNodesFreeList;

    /// Set of pointers to the arrays of preAllocated timer nodes.
    /// Used to delete the allocated memory when required.
    std::set<TimerNodeT<EventHandler*> *> m_PreAllocatedNodesSet;
};

typedef TimerHeapT<EventHandler*, EventHandlerTimeoutCallback, SYNCH_RECURSIVE_MUTEX> TimerHeap;

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::TimerHeapIteratorT( Heap &heap )
    : m_TimerHeap( heap )
{
    this->GetFirst();
}
template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::~TimerHeapIteratorT( void )
{
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetFirst( void )
{
    this->m_Position = 0;
}
template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetNext( void )
{
    if( this->m_Position != this->m_TimerHeap.m_Size )
    {
        ++this->m_Position;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
bool TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::IsDone( void ) const
{
    return this->m_Position == this->m_TimerHeap.m_Size;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE>* TimerHeapIteratorT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetCurrentNode( void )
{
    if( this->m_Position != this->m_TimerHeap.m_Size )
    {
        return this->m_TimerHeap.m_pHeap[this->m_Position];
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
inline size_t GetHeapParent (size_t X)
{
    return (X == 0 ? 0 : ((X - 1) / 2));
}

inline size_t GetHeapLeftChild (size_t X)
{
    return X + X + 1;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::TimerHeapT( size_t capacity
                                                        , bool bPreAllocated
                                                        , FUNCTOR *pCallbackFunctor
                                                        , FreeList<TimerNodeT<TYPE> > *pFreeList
                                                        , TIME_POLICY const &timePolicy )
    : BaseTimePolicy( pCallbackFunctor, pFreeList, timePolicy )
    , m_Capacity( capacity ), m_Size( 0 ), m_LimboSize( 0 ), m_CurrTimerIds( 0 )
    , m_TimerIdsMinFree( 0 ), m_pPreAllocatedNodes( NULL ), m_pPreAllocatedNodesFreeList( NULL )
{
    if( capacity > static_cast<size_t>( NumericLimits<long>::Max() ) )
    {
        capacity = static_cast<size_t>( NumericLimits<long>::Max() );
        this->m_Capacity = capacity;
    }

    NEW( this->m_pHeap, TimerNodeT<TYPE>*[capacity] );
    NEW( this->m_pTimerIds, ssize_t[capacity] );

    for( int i=0; i<capacity; ++i )
    {
        this->m_pTimerIds[i] = -1;
    }

    if( bPreAllocated )
    {
        NEW( this->m_pPreAllocatedNodes, TimerNodeT<TYPE>[capacity] );
        this->m_PreAllocatedNodesSet.insert( this->m_pPreAllocatedNodes );

        for( int j=1; j<capacity; ++j )
        {
            this->m_pPreAllocatedNodes[j-1].SetNext( &this->m_pPreAllocatedNodes[j] );
        }
        this->m_pPreAllocatedNodes[capacity-1].SetNext( NULL );
        this->m_pPreAllocatedNodesFreeList = &this->m_pPreAllocatedNodes[0];
    }
    NEW( this->m_pIterator, HeapIterator(*this) );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::TimerHeapT( FUNCTOR *pCallbackFunctor
                                                        , FreeList<TimerNodeT<TYPE> > *pFreeList
                                                        , TIME_POLICY const &timePolicy )
    : BaseTimePolicy( pCallbackFunctor, pFreeList, timePolicy )
    , m_Capacity( DEFAULT_TIMERS ), m_Size( 0 ), m_LimboSize( 0 ), m_CurrTimerIds( 0 )
    , m_TimerIdsMinFree( 0 ), m_pPreAllocatedNodes( NULL ), m_pPreAllocatedNodesFreeList( NULL )
{
    if( this->m_Capacity > NumericLimits<short>::Max() )
    {
        this->m_Capacity = static_cast<size_t>( NumericLimits<short>::Max() );
    }

    NEW( this->m_pHeap, TimerNodeT<TYPE>*[this->m_Capacity] );
    NEW( this->m_pTimerIds, ssize_t[this->m_Capacity] );

    for( int i=0; i<this->m_Capacity; ++i )
    {
        this->m_pTimerIds[i] = -1;
    }

    NEW( this->m_pIterator, HeapIterator(*this) );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::~TimerHeapT( void )
{
    delete this->m_pIterator;
    this->Close();

    delete[] this->m_pHeap;
    delete[] this->m_pTimerIds;

    if( this->m_pPreAllocatedNodes != NULL )
    {
        std::set<TimerNodeT<EventHandler*> *>::iterator iterator;
        iterator = this->m_PreAllocatedNodesSet.begin();
        for( ; iterator != this->m_PreAllocatedNodesSet.end(); ++iterator )
        {
            delete[] *iterator;
        }
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
bool TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::IsEmpty( void) const
{
    return this->m_Size == 0;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
const TimeValue & TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetEarliestTime( void ) const
{
    return this->m_pHeap[0]->GetTimerValue();
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ResetInterval( long timerId, const TimeValue &interval )
{
    GUARD_RETURN( LOCK, guard, this->m_Lock, -1 );

    // Locate the ACE_Timer_Node that corresponds to the timerId.

    // Check to see if the timerId is out of range
    if( timerId < 0 || (size_t) timerId > this->m_Capacity )
    {
        return -1;
    }

    ssize_t timer_node_slot = this->m_pTimerIds[timerId];

    // Check to see if timerId is still valid.
    if( timer_node_slot < 0 )
    {
        return -1;
    }

    if( timerId != this->m_pHeap[timer_node_slot]->GetTimerId() )
    {
        return -1;
    }
    else
    {
        // Reset the timer interval
        this->m_pHeap[timer_node_slot]->SetTimerInterval( interval );
        return 0;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Cancel( const TYPE &node, int dontCallHandleClose )
{
    GUARD_RETURN( LOCK, guard, this->m_Lock, -1 );

    int iNumOfCancellations = 0;

    // Try to locate the ACE_Timer_Node that matches the timerId.

    cout<<__FILE__<<"_"<<__LINE__<<"__TimerHeapT<...>::Cancel this->m_Size="<<this->m_Size<<endl;
    for( size_t i = 0; i < this->m_Size; )
    {
        if( this->m_pHeap[i]->GetNode () == node )
        {
            TimerNodeT<TYPE> *temp = this->Remove( i );

            ++iNumOfCancellations;

            this->FreeNode( temp );

            // We reset to zero so that we don't miss checking any nodes
            // if a reheapify occurs when a node is removed.  There
            // may be a better fix than this, however.
            i = 0;
        }
        else
        {
            ++i;
        }
    }

    // Call the close hooks.
    int cookie = 0;

    // CancelNode() called once per <node>.
    this->GetCallbackFunctor().CancelNode( *this, node, dontCallHandleClose, cookie );

    for (int j = 0; j < iNumOfCancellations; ++j )
    {
        // cancel_timer() called once per <timer>.
         this->GetCallbackFunctor().CancelTimer( *this, node, dontCallHandleClose, cookie );
    }

    return iNumOfCancellations;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Cancel( long timerId, const void **pAct, int dontCallHandleClose )
{
    GUARD_RETURN( LOCK, guard, this->m_Lock, -1 );

    // Locate the ACE_Timer_Node that corresponds to the timerId.

    // Check to see if the timerId is out of range
    if( timerId < 0 || (size_t)timerId > this->m_Capacity )
    {
        return 0;
    }

    ssize_t timer_node_slot = this->m_pTimerIds[timerId];

    // Check to see if timerId is still valid.
    if( timer_node_slot < 0 )
    {
        return 0;
    }

    if( timerId != this->m_pHeap[timer_node_slot]->GetTimerId() )
    {
        //ACE_ASSERT (timerId == this->m_pHeap[timer_node_slot]->GetTimerId ());
        return 0;
    }
    else
    {
        TimerNodeT<TYPE> *temp = this->Remove( timer_node_slot );

        // Call the close hooks.
        int cookie = 0;

        // cancel_type() called once per <node>.
        this->GetCallbackFunctor().CancelNode( *this, temp->GetNode(), dontCallHandleClose, cookie);

        // cancel_timer() called once per <timer>.
        this->GetCallbackFunctor().CancelTimer( *this, temp->GetNode(), dontCallHandleClose, cookie);

        if( pAct != 0 )
        {
            *pAct = temp->GetAct();
        }

        this->FreeNode( temp );
        return 1;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
int TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Close( void)
{
    size_t current_size = this->m_Size;

    // Clean up all the nodes still in the queue
    for( size_t i = 0; i < current_size; ++i )
    {
        // Grab the event_handler and act, then delete the node before calling
        // back to the handler. Prevents a handler from trying to cancel_timer()
        // inside handle_close(), ripping the current timer node out from
        // under us.
        TYPE eh = this->m_pHeap[i]->GetNode ();
        const void *act = this->m_pHeap[i]->GetAct();
        this->FreeNode( this->m_pHeap[i] );
        this->GetCallbackFunctor().Deletion( *this, eh, act );
    }
    // leave the rest to the destructor
    return 0;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerQueueIteratorT<TYPE> & TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetIterator( void )
{
    this->m_pIterator->GetFirst();
    return *this->m_pIterator;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE> * TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::RemoveFirstNode( void )
{
    if( 0 == this->m_Size )
    {
        return NULL;
    }

    return this->Remove( 0 );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE> * TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetFirstNode( void )
{
    return 0==this->m_Size ? NULL : this->m_pHeap[0];
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
long TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ScheduleAux( const TYPE &node
                                                                , const void *pAct
                                                                , const TimeValue &futureTime
                                                                , const TimeValue &interval )
{
    if( (this->m_Size + this->m_LimboSize) < this->m_Capacity )
    {
        // Obtain the next unique sequence number.
        long const timerId = this->GetTimerId();

        // Obtain the memory to the new node.
        TimerNodeT<TYPE> *temp = 0;

        ALLOCATOR_RETURN (temp, this->AllocNode(), -1 );
        temp->Init( node, pAct, futureTime, interval, NULL, timerId );

        this->Insert( temp );
        return timerId;
    }
    else
        return -1;

}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ReSchedule( TimerNodeT<TYPE> *expired )
{
    if( -2 == this->m_pTimerIds[expired->GetTimerId()] )
    {
        --this->m_LimboSize;
    }
    this->Insert( expired );

}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE> * TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::AllocNode( void )
{
    TimerNodeT<TYPE> *temp = 0;

    // Only allocate a node if we are *not* using the preallocated heap.
    if ( NULL == this->m_pPreAllocatedNodes )
    {
        NEW_RETURN( temp, TimerNodeT<TYPE>, 0 );
    }
    else
    {
        // check to see if the heap needs to grow
        if( NULL == this->m_pPreAllocatedNodesFreeList )
        {
            this->GrowHeap();
        }

        temp = this->m_pPreAllocatedNodesFreeList;

        if( this->m_pPreAllocatedNodesFreeList != NULL )
        {
            // Remove the first element from the freelist.
            this->m_pPreAllocatedNodesFreeList = this->m_pPreAllocatedNodesFreeList->GetNext();
        }
    }
    return temp;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::FreeNode( TimerNodeT<TYPE> *node )
{
    // Return this timer id to the freelist.
    this->PushFreeList( node->GetTimerId() );

    // Only free up a node if we are *not* using the preallocated heap.
    if( NULL == this->m_pPreAllocatedNodes )
    {
        delete node;
    }
    else
    {
        node->SetNext( this->m_pPreAllocatedNodesFreeList );
        this->m_pPreAllocatedNodesFreeList = node;
    }
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
TimerNodeT<TYPE> * TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Remove( size_t slot )
{
    TimerNodeT<TYPE> *pRemovedNode = this->m_pHeap[slot];

    // NOTE - the m_Size is being decremented since the queue has one
    // less active timer in it. However, this ACE_Timer_Node is not being
    // freed, and there is still a place for it in m_pTimerIds (the timer ID
    // is not being relinquished). The node can still be rescheduled, or
    // it can be freed via free_node.
    --this->m_Size;

    // Only try to reheapify if we're not deleting the last entry.

    if (slot < this->m_Size)
    {
        TimerNodeT<TYPE> *pMovedNode = this->m_pHeap[this->m_Size];

        // Move the end node to the location being removed and update
        // the corresponding slot in the parallel <timer_ids> array.
        this->Copy( slot, pMovedNode );

        // If the <pMovedNode->time_value_> is great than or equal its
        // parent it needs be moved down the heap.
        size_t parent = GetHeapParent( slot );

        if (pMovedNode->GetTimerValue () >= this->m_pHeap[parent]->GetTimerValue ())
            this->ReHeapDown( pMovedNode, slot, GetHeapLeftChild(slot) );
        else
            this->ReHeapUp( pMovedNode, slot, parent );
    }

    this->m_pTimerIds[pRemovedNode->GetTimerId()] = -2;
    ++this->m_LimboSize;
    return pRemovedNode;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Insert( TimerNodeT<TYPE> *pNewNode )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__TimerHeapT<...>::Insert() begin!"<<endl;

    if( this->m_Size + this->m_LimboSize + 2 >= this->m_Capacity )
    {
        this->GrowHeap();
    }

    this->ReHeapUp( pNewNode, this->m_Size, GetHeapParent(this->m_Size) );
    ++this->m_Size;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GrowHeap( void )
{
    // All the containers will double in size from m_Capacity.
    size_t new_size = this->m_Capacity * 2;

    // First grow the heap itself.
    TimerNodeT<TYPE> **new_heap = 0;

    NEW (new_heap,
             TimerNodeT<TYPE> *[new_size]);

    memcpy (new_heap, this->m_pHeap, this->m_Capacity * sizeof *new_heap );
    delete [] this->m_pHeap;
    this->m_pHeap = new_heap;

    // Grow the array of timer ids.

    ssize_t *new_timer_ids = 0;

    NEW (new_timer_ids, ssize_t[new_size]);

    memcpy (new_timer_ids, this->m_pTimerIds, this->m_Capacity * sizeof (ssize_t));

    delete [] this->m_pTimerIds;
    this->m_pTimerIds = new_timer_ids;

    // And add the new elements to the end of the "freelist".
    for (size_t i = this->m_Capacity; i < new_size; ++i)
        this->m_pTimerIds[i] = -(static_cast<ssize_t> (i) + 1);

    // Grow the preallocation array (if using preallocation)
    if (this->m_pPreAllocatedNodes != 0)
    {
        // Create a new array with max_size elements to link in to
        // existing list.
        NEW (this->m_pPreAllocatedNodes, TimerNodeT<TYPE>[this->m_Capacity]);

        // Add it to the set for later deletion
        this->m_PreAllocatedNodesSet.insert( this->m_pPreAllocatedNodes );

        // Link new nodes together (as for original list).
        for (size_t k = 1; k < this->m_Capacity; ++k)
        {
            this->m_pPreAllocatedNodes[k - 1].SetNext( &this->m_pPreAllocatedNodes[k] );
        }

        // NULL-terminate the new list.
        this->m_pPreAllocatedNodes[this->m_Capacity - 1].SetNext (0);

        // Link new array to the end of the existling list.
        if (this->m_pPreAllocatedNodesFreeList == 0)
        {
            this->m_pPreAllocatedNodesFreeList = &this->m_pPreAllocatedNodes[0];
        }
        else
        {
            TimerNodeT<TYPE> *previous = this->m_pPreAllocatedNodesFreeList;

            for (TimerNodeT<TYPE> *current = this->m_pPreAllocatedNodesFreeList->GetNext(); current != 0; current = current->GetNext() )
            {
                previous = current;
            }

            previous->SetNext( &this->m_pPreAllocatedNodes[0] );
        }
    }

    this->m_Capacity = new_size;
    // Force rescan of list from beginning for a free slot (I think...)
    // This fixed Bugzilla #2447.
    this->m_TimerIdsMinFree = this->m_Capacity;
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ReHeapUp( TimerNodeT<TYPE> *pNewNode, size_t slot, size_t parent )
{
    while (slot > 0)
    {
        // If the parent node is greater than the <pMovedNode> we need
        // to copy it down.
        if (pNewNode->GetTimerValue() < this->m_pHeap[parent]->GetTimerValue () )
        {
            this->Copy( slot, this->m_pHeap[parent] );
            slot = parent;
            parent = GetHeapParent( slot );
        }
        else
        {
            break;
        }
    }

    // Insert the new node into its proper resting place in the heap and
    // update the corresponding slot in the parallel <timer_ids> array.
    this->Copy( slot, pNewNode );

}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::ReHeapDown( TimerNodeT<TYPE> *pMovedNode, size_t slot, size_t child )
{
    while (child < this->m_Size)
    {
        // Choose the smaller of the two children.
        if (child + 1 < this->m_Size && this->m_pHeap[child + 1]->GetTimerValue() < this->m_pHeap[child]->GetTimerValue() )
        {
            ++child;
        }

        // Perform a <copy> if the child has a larger timeout value than
        // the <pMovedNode>.
        if (this->m_pHeap[child]->GetTimerValue() < pMovedNode->GetTimerValue() )
        {
            this->Copy( slot, this->m_pHeap[child] );
            slot = child;
            child = GetHeapLeftChild( child );
        }
        else
        {
            // We've found our location in the heap.
            break;
        }
    }
    this->Copy ( slot, pMovedNode );
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::Copy( size_t slot, TimerNodeT<TYPE> *pMovedNode )
{
    // Insert <pMovedNode> into its new location in the heap.
    this->m_pHeap[slot] = pMovedNode;

    //ACE_ASSERT (pMovedNode->GetTimerId () >= 0 && pMovedNode->GetTimerId() < (int) this->m_Capacity);

    // Update the corresponding slot in the parallel <m_pTimerIds> array.
    this->m_pTimerIds[pMovedNode->GetTimerId ()] = static_cast<ssize_t> (slot);
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
long TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::GetTimerId( void )
{
    return this->PopFreeList();
}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
long TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::PopFreeList( void )
{
    // Scan for a free timer ID. Note that since this function is called
    // _after_ the check for a full timer heap, we are guaranteed to find
    // a free ID, even if we need to wrap around and start reusing freed IDs.
    // On entry, the curr_ index is at the previous ID given out; start
    // up where we left off last time.
    // NOTE - a m_pTimerIds slot with -2 is out of the heap, but not freed.
    // It must be either freed (free_node) or rescheduled (reschedule).
    ++this->m_CurrTimerIds;
    while (this->m_CurrTimerIds < this->m_Capacity &&
           (this->m_pTimerIds[this->m_CurrTimerIds] >= 0 ||
            this->m_pTimerIds[this->m_CurrTimerIds] == -2  ))
        ++this->m_CurrTimerIds;
    if (this->m_CurrTimerIds == this->m_Capacity)
    {
        //ACE_ASSERT (this->m_TimerIdsMinFree < this->m_Capacity);
        this->m_CurrTimerIds = this->m_TimerIdsMinFree;
        // We restarted the free search at min. Since min won't be
        // free anymore, and curr_ will just keep marching up the list
        // on each successive need for an ID, reset min_free_ to the
        // size of the list until an ID is freed that curr_ has already
        // gone past (see push_freelist).
        this->m_TimerIdsMinFree = this->m_Capacity;
    }

    return static_cast<long> (this->m_CurrTimerIds);

}

template<class TYPE, class FUNCTOR, class LOCK, class TIME_POLICY>
void TimerHeapT<TYPE, FUNCTOR, LOCK, TIME_POLICY>::PushFreeList( long old_id )
{
    // Since this ID has already been checked by one of the public
    // functions, it's safe to cast it here.
    size_t oldid = static_cast<size_t> (old_id);

    // The freelist values in the <m_pTimerIds> are negative, so set the
    // freed entry back to 'free'. If this is the new lowest value free
    // timer ID that curr_ won't see on it's normal march through the list,
    // remember it.
    //ACE_ASSERT (this->m_pTimerIds[oldid] >= 0 || this->m_pTimerIds[oldid] == -2);
    if (this->m_pTimerIds[oldid] == -2 )
    {
        --this->m_LimboSize;
    }
    else
    {
        --this->m_Size;
    }
    this->m_pTimerIds[oldid] = -1;
    if (oldid < this->m_TimerIdsMinFree && oldid <= this->m_CurrTimerIds)
    {
        this->m_TimerIdsMinFree = oldid;
    }
    return;
}

#endif

