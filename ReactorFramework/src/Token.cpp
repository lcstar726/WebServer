#include "Token.h"
#include "SynchTraits.h"

Token::TokenQueueEntry::TokenQueueEntry( ThreadMutex &mutex, thread_t threadId )
    : m_pNextEntry( NULL ), m_ThreadId( threadId ), m_iRunable( 0 )
{
}
int Token::TokenQueueEntry::Wait( TimeValue *timeOut, ThreadMutex &lock )
{
    lock.Release();

    int const iRet = ( NULL==timeOut ? this->m_CV.Acquire() : this->m_CV.Acquire(timeOut) );

    lock.Acquire();

    return iRet;
}
int Token::TokenQueueEntry::Signal( void )
{
    return this->m_CV.Release();
}

Token::TokenQueue::TokenQueue( void )
    : m_pHead( NULL ), m_pTail( NULL )
{
}

void Token::TokenQueue::RemoveEntry( TokenQueueEntry *entry )
{
    TokenQueueEntry *currEntry = this->m_pHead;
    TokenQueueEntry *prevEntry = NULL;

    for( ; currEntry!=NULL && currEntry != entry; currEntry = currEntry->m_pNextEntry )
    {
        prevEntry = currEntry;
    }

    if( NULL == currEntry )
    {
        return;
    }
    else if( NULL == prevEntry )
    {
        this->m_pHead = currEntry->m_pNextEntry;
    }
    else
    {
        prevEntry->m_pNextEntry = currEntry->m_pNextEntry;
    }

    if( NULL == currEntry->m_pNextEntry )
    {
        this->m_pTail = prevEntry;
    }
}

void Token::TokenQueue::InsertEntry( TokenQueueEntry &entry, int iRequeuePosition )
{
    if( NULL == this->m_pHead )
    {
        this->m_pHead = &entry;
        this->m_pTail = &entry;
    }
    else if( 0 == iRequeuePosition )
    {
        entry.m_pNextEntry = this->m_pHead;
        this->m_pHead = &entry;
    }
    else if( -1 == iRequeuePosition )
    {
        this->m_pTail->m_pNextEntry = &entry;
        this->m_pTail = &entry;
    }
    else
    {
        TokenQueueEntry *currEntry = this->m_pHead;
        for( ;iRequeuePosition-- && currEntry->m_pNextEntry != NULL; )
        {
            currEntry = currEntry->m_pNextEntry;
        }
        entry.m_pNextEntry = currEntry->m_pNextEntry;
        currEntry->m_pNextEntry = &entry;

        if( NULL == entry.m_pNextEntry )
        {
            this->m_pTail = &entry;
        }
    }
    
}

Token::Token( mutexattr_t *any )
    : m_Lock( any )
    , m_iInUse( 0 )
    , m_iWaiters( 0 )
    , m_iNestingLevel( 0 )
    , m_iQueueingStrategy( Token::FIFO )
{
}

Token::~Token( void )
{
}

int Token::SharedAcquire( void (*SleepHookFunc)(void*), void *arg, TimeValue *timeOut, eTokenOpType opType )
{
    const thread_t thrId = Thread::GetThreadSelf();

    if( !this->m_iInUse )
    {
        this->m_iInUse = opType;
        this->m_Owner = thrId;
        return 0;
    }

    if( Thread::ThreadEqual(thrId, this->m_Owner) )
    {
        ++this->m_iNestingLevel;
        return 0;
    }

    if( timeOut != NULL && *timeOut == TimeValue::zero )
    {
        errno = ETIME;
        return -1;
    }

    Token::TokenQueueEntry newEntry( this->m_Lock, thrId );
    Token::TokenQueue *queue = ( opType == Token::READ_TOKEN ? &this->m_ReadQueue : &this->m_WriterQueue );

    queue->InsertEntry( newEntry, this->m_iQueueingStrategy );
    ++this->m_iWaiters;

    int iRet = 0;
    if( SleepHookFunc )
    {
        (*SleepHookFunc)( arg );
        ++iRet;
    }
    else
    {
        this->SleepHook();
        ++iRet;
    }

    bool bIsTimeOut = false;
    bool bIsError = false;

    do
    {
        int const iResult = newEntry.Wait( timeOut, this->m_Lock );
        if( -1 == iResult )
        {
            if( EINTR == errno )
            {
                continue;
            }
            else if( ETIME == errno )
            {
                bIsTimeOut = true;
            }
            else
            {
                bIsError = true;
            }
            break;
        }
    }
    while( !Thread::ThreadEqual(thrId, this->m_Owner) );

    --this->m_iWaiters;
    queue->RemoveEntry( &newEntry );

    if( bIsTimeOut )
    {
        if( newEntry.m_iRunable )
        {
            this->WakeupNextWaiter();
        }
        return -1;
    }
    else if( bIsError )
    {
        return -1;
    }

    return iRet;
}

void Token::WakeupNextWaiter( void )
{
    // Reset state for new owner.
    this->m_Owner = Thread::NullThread;
    this->m_iInUse = 0;

    // Any waiters...
    if(this->m_WriterQueue.m_pHead == 0 &&
       this->m_ReadQueue.m_pHead == 0)
    {
        // No more waiters...
        return;
    }

    // Wakeup next waiter.
    TokenQueue *queue = NULL;

    // Writer threads get priority to run first.
    if(this->m_WriterQueue.m_pHead != NULL )
    {
        this->m_iInUse = Token::WRITE_TOKEN;
        queue = &this->m_WriterQueue;
    }
    else
    {
        this->m_iInUse = Token::READ_TOKEN;
        queue = &this->m_ReadQueue;
    }

    // Wake up waiter and make it runable.
    queue->m_pHead->m_iRunable = 1;
    queue->m_pHead->Signal();
    this->m_Owner = queue->m_pHead->m_ThreadId;
}


int Token::Acquire( TimeValue *timeOut )
{
    return this->SharedAcquire( NULL, NULL, timeOut, Token::WRITE_TOKEN );
}

int Token::Acquire( void(*SleepHookFunc)(void*), void *arg, TimeValue *timeOut )
{
    return this->SharedAcquire( SleepHookFunc, arg, timeOut, Token::WRITE_TOKEN );
}

int Token::TryAcquire( void )
{
    return this->SharedAcquire( NULL, NULL, (TimeValue*)&TimeValue::zero, Token::WRITE_TOKEN );
}

int Token::Remove( void )
{    
    // Don't have an implementation for this yet...
    return -1;
}

int Token::Release( void )
{
    GUARD_RETURN( SYNCH_MUTEX, guard, this->m_Lock, -1 );
    if( this->m_iNestingLevel > 0 )
    {
        --this->m_iNestingLevel;
    }
    else
    {
        this->WakeupNextWaiter();
    }
    return 0;
}

void Token::SleepHook( void )
{
}

thread_t Token::GetCurrentOwner( void )
{
    return this->m_Owner;
}


int Token::GetQueueingStrategy( void )
{
    return this->m_iQueueingStrategy;
}

void Token::SetQueueingStrategy( int iStrategy )
{
    this->m_iQueueingStrategy = iStrategy;
}

inline int Token::GetWaiters( void )
{
    return this->m_iWaiters;
}

