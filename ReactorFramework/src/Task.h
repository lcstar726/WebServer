#ifndef __TASK_H__
#define __TASK_H__

#include "SynchTraits.h"
#include "SystemTime.h"
#include "MessageBlock.h"
#include "ThreadManager.h"
#include "TaskBase.h"
#include "MessageQueue.h"

class TaskFlags
{
    enum
    {
        READER      = 01
        , FLUSHDATA   = 02
        , FLUSHALL    = 04
    };
};

template<SYNCH_DECL, class TIME_POLICY = SystemTimePolicy>
class Task: public TaskBase
{
public:
    Task( ThreadManager *thrMgr = 0
            , MessageQueue<SYNCH_USE, TIME_POLICY> *msgQueue = 0 );
    
    ~Task( void );
    
    int PutQ( MessageBlock *msgBlk, TimeValue *timeout = 0 );
    
    int GetQ( MessageBlock *&msgBlk, TimeValue *timeout = 0 );
    
    Task<SYNCH_USE, TIME_POLICY>* GetNextTask( void );
    
    void SetNextTask( Task<SYNCH_USE, TIME_POLICY> *newTask );
    
protected:
    MessageQueue<SYNCH_USE, TIME_POLICY> *m_MsgQueue;
    
    bool m_ShoudDeleteMsgQueue;
    
    Task<SYNCH_USE, TIME_POLICY> *m_NextTask;

private:
    Task( const Task<SYNCH_USE, TIME_POLICY> & );
    Task<SYNCH_USE, TIME_POLICY>& operator= ( const Task<SYNCH_USE, TIME_POLICY> & );
    
};

template<SYNCH_DECL, class TIME_POLICY>
Task<SYNCH_USE, TIME_POLICY>::Task( ThreadManager *thrMgr, MessageQueue<SYNCH_USE, TIME_POLICY> *msgQueue )
    : TaskBase( thrMgr ), m_MsgQueue( msgQueue ), m_ShoudDeleteMsgQueue( false ), m_NextTask( 0 )
{
    typedef MessageQueue<SYNCH_USE, TIME_POLICY> QUEUE_TYPE;
    
    if( 0 == msgQueue )
    {
        NEW( msgQueue, QUEUE_TYPE );
        
        this->m_ShoudDeleteMsgQueue = true;
    }
    this->m_MsgQueue = msgQueue;
}

template<SYNCH_DECL, class TIME_POLICY>
Task<SYNCH_USE, TIME_POLICY>::~Task( void )
{
    if( this->m_ShoudDeleteMsgQueue )
    {
        delete this->m_MsgQueue;
    }
    
    this->m_ShoudDeleteMsgQueue = false;
}

template<SYNCH_DECL, class TIME_POLICY>
inline int Task<SYNCH_USE, TIME_POLICY>::GetQ( MessageBlock *&msgBlk, TimeValue *timeout )
{
    return this->m_MsgQueue->DequeueHead( msgBlk, timeout );
}

template<SYNCH_DECL, class TIME_POLICY>
inline int Task<SYNCH_USE, TIME_POLICY>::PutQ( MessageBlock *msgBlk, TimeValue *timeout )
{
    return this->m_MsgQueue->EnqueueTail( msgBlk, timeout );
}

template<SYNCH_DECL, class TIME_POLICY>
inline void Task<SYNCH_USE, TIME_POLICY>::SetNextTask( Task<SYNCH_USE, TIME_POLICY> *newTask )
{
    this->m_NextTask = newTask;
}

template<SYNCH_DECL, class TIME_POLICY>
inline Task<SYNCH_USE, TIME_POLICY>* Task<SYNCH_USE, TIME_POLICY>::GetNextTask( void )
{
    return this->m_NextTask;
}

#endif

