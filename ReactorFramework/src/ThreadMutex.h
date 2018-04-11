#ifndef __THREAD_MUTEX_H__
#define __THREAD_MUTEX_H__

#include "Public.h"
#include "GlobalMacros.h"

class ThreadMutex
{
public:
    /// Constructor.
    ThreadMutex( mutexattr_t *attributes = 0);

    ~ThreadMutex(void);

    int Remove(void);

    int Acquire(void);

    int TryAcquire(void);

    int Release(void);


    const thread_mutex_t &GetLock(void) const;
    thread_mutex_t &GetLock(void);

protected:
    thread_mutex_t m_Lock;

    bool m_Removed;

private:
    void operator= (const ThreadMutex &);
    ThreadMutex (const ThreadMutex &);
};

class RecursiveThreadMutex
{
public:
    RecursiveThreadMutex( mutexattr_t *arg = 0 );
    
    ~RecursiveThreadMutex( void );
    
    int Remove( void );
    
    int Acquire( void );
    
    int TryAcquire( void );
    
    int Release( void );
    
    recursive_thread_mutext_t& GetLock( void );
    
    //thread_t GetThreadId( void );
    
    //int GetNestingLevel( void );
    
    thread_mutex_t& GetNestingMutex( void );

protected:
    recursive_thread_mutext_t m_Lock;
    
    bool m_IsRemoved;
    
    void SetThreadId( thread_t thrId );

private:
    RecursiveThreadMutex( const RecursiveThreadMutex & lock );
    void operator= ( const RecursiveThreadMutex & lock );
};

class StaticObjectLock
{
public:
    static RecursiveThreadMutex* GetInstance( void );
    static void CleanupClock( void );
};

#endif

