#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include "Public.h"
#include "MessageBlock.h"
#include "SynchTraits.h"

template<SYNCH_DECL, class TIME_POLICY>
class MessageQueueIterator;

template<SYNCH_DECL, class TIME_POLICY>
class MessageQueueReverseIterator;

class MessageQueueBase
{
public:
    enum
    {
        DEFAULT_HWM = 16 * 1024,
        DEFAULT_LWM = 16 * 1024,
        
        ACTIVATED = 1,
        DEACTIVATED = 2,
        PULSED = 3
    };
    
    MessageQueueBase( void ) { }
    
    virtual ~MessageQueueBase( void ) { }
    
    virtual int PeekDequeueHead( MessageBlock *firstItem, TimeValue *timeout = 0 ) = 0;

    virtual int EnqueueTail( MessageBlock *newItem, TimeValue *timeout = 0 ) = 0;
    virtual int Enqueue( MessageBlock *newItem, TimeValue *timeout = 0 ) = 0;

    virtual int DequeueHead( MessageBlock *&firstItem, TimeValue *timeout = 0 ) = 0;
    virtual int Dequeue( MessageBlock *&firstItem, TimeValue *timeout = 0 ) = 0;

protected:
    int m_State;

private:
    MessageQueueBase( const MessageQueueBase & );
    void operator= ( const MessageQueueBase & );
};

template<SYNCH_DECL, class TIME_POLICY = SystemTimePolicy>
class MessageQueue: public MessageQueueBase
{
public:
    friend class MessageQueueIterator<SYNCH_USE, TIME_POLICY>;
    friend class MessageQueueReverseIterator<SYNCH_USE, TIME_POLICY>;
    
    typedef MessageQueueIterator<SYNCH_USE, TIME_POLICY> tIterator;
    typedef MessageQueueReverseIterator<SYNCH_USE, TIME_POLICY> tReverseIterator;
    
    MessageQueue( size_t hwm = MessageQueueBase::DEFAULT_HWM
                , size_t lwm = MessageQueueBase::DEFAULT_LWM );
    ~MessageQueue( void );

    virtual int Open( size_t hwm = MessageQueueBase::DEFAULT_HWM
                , size_t lwm = MessageQueueBase::DEFAULT_LWM );
     
    virtual int PeekDequeueHead( MessageBlock *firstItem, TimeValue *timeout = 0 );
    
    virtual int EnqueueTail( MessageBlock *newItem, TimeValue *timeout = 0 );
    virtual int Enqueue( MessageBlock *newItem, TimeValue *timeout = 0 );
    
    virtual int DequeueHead( MessageBlock *&firstItem, TimeValue *timeout = 0 );
    virtual int Dequeue( MessageBlock *&firstItem, TimeValue *timeout=0 );

protected:
    MessageBlock *m_pHead;
    MessageBlock *m_pTail;
    
    size_t m_HighWaterMark;
    size_t m_LowWaterMark;
    
    size_t m_CurrentBytes;
    size_t m_CurrentLength;
    size_t m_CurrentCount;
    
    SYNCH_MUTEX_T m_Lock;

private:
    MessageQueue( const MessageQueue<SYNCH_USE, TIME_POLICY>& msgQ );
    void operator= ( const MessageQueue<SYNCH_USE, TIME_POLICY>& msgQ );
};

template<SYNCH_DECL, class TIME_POLICY>
MessageQueue<SYNCH_USE, TIME_POLICY>::MessageQueue( size_t hwm,size_t lwm )
{
    this->Open( hwm, lwm );
}

template<SYNCH_DECL, class TIME_POLICY>
MessageQueue<SYNCH_USE, TIME_POLICY>::~MessageQueue( void )
{
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::Open( size_t hwm,size_t lwm )
{
    this->m_HighWaterMark = hwm;
    this->m_LowWaterMark  = lwm;
    this->m_State = MessageQueueBase::ACTIVATED;
    this->m_CurrentBytes = 0;
    this->m_CurrentLength = 0;
    this->m_CurrentCount = 0;
    this->m_pTail = 0;
    this->m_pHead = 0;
    
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::PeekDequeueHead( MessageBlock *firstItem, TimeValue *timeout )
{
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::EnqueueTail( MessageBlock *newItem, TimeValue *timeout )
{
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::Enqueue( MessageBlock *newItem, TimeValue *timeout )
{
    return 0;
}
    
template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::DequeueHead( MessageBlock *&firstItem, TimeValue *timeout )
{
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueue<SYNCH_USE, TIME_POLICY>::Dequeue( MessageBlock *&firstItem, TimeValue *timeout )
{
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
class MessageQueueIterator
{
public:
    MessageQueueIterator( MessageQueue<SYNCH_USE, TIME_POLICY> &queue );
    
    int Next( MessageBlock *&entry );
    
    int Advance( void );
    
    int Done( void ) const;
    
private:
    MessageQueue<SYNCH_USE, TIME_POLICY> &m_MsgQueue;
    
    MessageBlock *m_CurrentMsgBlk;
};

template<SYNCH_DECL, class TIME_POLICY>
MessageQueueIterator<SYNCH_USE, TIME_POLICY>::MessageQueueIterator( MessageQueue<SYNCH_USE, TIME_POLICY> &queue )
    : m_MsgQueue( queue ), m_CurrentMsgBlk( m_MsgQueue.m_pHead )
{
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueueIterator<SYNCH_USE, TIME_POLICY>::Next( MessageBlock *&entry )
{
    READ_GUARD_RETURN( SYNCH_MUTEX_T, guardObj, this->m_MsgQueue.m_Lock, -1 )

    if( this->m_CurrentMsgBlk != 0 )
    {
        entry = this->m_CurrentMsgBlk;
        
        return 1;
    }
    return 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueueIterator<SYNCH_USE, TIME_POLICY>::Advance( void )
{
    READ_GUARD_RETURN( SYNCH_MUTEX_T, guardObj, this->m_MsgQueue.m_Lock, -1 )
    
    if( this->m_CurrentMsgBlk )
    {
        this->m_CurrentMsgBlk = this->m_CurrentMsgBlk->GetNext();
    }
    
    return this->m_CurrentMsgBlk != 0;
}

template<SYNCH_DECL, class TIME_POLICY>
int MessageQueueIterator<SYNCH_USE, TIME_POLICY>::Done( void ) const
{
    READ_GUARD_RETURN( SYNCH_MUTEX_T, guardObj, this->m_MsgQueue.m_Lock, -1 )
    
    return 0 == this->m_CurrentMsgBlk;
}
#endif
