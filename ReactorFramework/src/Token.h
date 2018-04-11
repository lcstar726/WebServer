#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "Public.h"
#include "Thread.h"
#include "ThreadMutex.h"
#include "Semaphore.h"
#include "TimeValue.h"

class Token
{
public:
    enum eQueueingStrategy
    {
        FIFO = -1
        , LIFO = 0
    };

    struct TokenQueueEntry
    {
        TokenQueueEntry( ThreadMutex &mutex, thread_t threadId );

        int Wait( TimeValue *timeOut, ThreadMutex &lock );
        int Signal( void );

        TokenQueueEntry *m_pNextEntry;
        thread_t        m_ThreadId;
        Semaphore       m_CV;
        int             m_iRunable;
    };

    Token( mutexattr_t *attr = NULL );
    virtual ~Token( void );

    int Acquire( TimeValue *timeOut = NULL );
    int Acquire( void(*SleepHookFunc)(void*), void *arg, TimeValue *timeOut = NULL );
    int TryAcquire( void );
    int Remove( void );
    int Release( void );

    virtual void SleepHook( void );

    thread_t GetCurrentOwner( void );

    int GetQueueingStrategy( void );
    void SetQueueingStrategy( int iStrategy );
    int GetWaiters( void );

private:
    enum eTokenOpType
    {
        READ_TOKEN = 1
        , WRITE_TOKEN
    };

    struct TokenQueue
    {
        TokenQueue( void );

        void RemoveEntry( TokenQueueEntry * );
        void InsertEntry( TokenQueueEntry &entry, int iRequeuePosition = -1 );

        TokenQueueEntry     *m_pHead;
        TokenQueueEntry     *m_pTail;
    };

    int SharedAcquire( void (*SleepHookFunc)(void*), void *arg, TimeValue *timeOut, eTokenOpType opType );
    void WakeupNextWaiter( void );

    TokenQueue      m_WriterQueue;
    TokenQueue      m_ReadQueue;
    ThreadMutex     m_Lock;
    thread_t        m_Owner;
    int             m_iInUse;
    int             m_iWaiters;
    int             m_iNestingLevel;
    int             m_iQueueingStrategy;
};

#endif

