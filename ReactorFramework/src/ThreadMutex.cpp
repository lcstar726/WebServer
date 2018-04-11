#include "ThreadMutex.h"
#include "Cleanup.h"

ThreadMutex::ThreadMutex( mutexattr_t *attributes )
{
    pthread_mutex_init( &this->m_Lock, attributes );
}

ThreadMutex::~ThreadMutex( void )
{
    this->Remove();
}

int ThreadMutex::Remove( void )
{
    int result = 0;
    if( !m_Removed )
    {
        result = pthread_mutex_destroy( &this->m_Lock );
    }
    
    return result;
}

int ThreadMutex::Acquire( void )
{
    int result;
    return ADAPT_RETVAL( pthread_mutex_lock(&this->m_Lock), result );
}

int ThreadMutex::TryAcquire( void )
{
    int result = 0;
    return ADAPT_RETVAL( pthread_mutex_trylock(&this->m_Lock), result );
}

int ThreadMutex::Release( void )
{
    int result = 0;
    return ADAPT_RETVAL( pthread_mutex_unlock( &this->m_Lock ), result );
}

thread_mutex_t& ThreadMutex::GetLock( void )
{
    return this->m_Lock;
}


RecursiveThreadMutex::RecursiveThreadMutex( mutexattr_t *args ): m_IsRemoved( false )
{
    int iLockType = PTHREAD_MUTEX_RECURSIVE;
    
    mutexattr_t attrTemp;
    
    if( 0 == args )
    {
        args = &attrTemp;
    }
    
    int result = 0;
    bool iAttrInit = false;
    
    if( args == &attrTemp )
    {
        if( 0 == ADAPT_RETVAL( pthread_mutexattr_init(args), result ) )
        {
            iAttrInit = true;
        }
        else
        {
            result = -1;
        }
    }
    
    if( 0 == result && iLockType != 0 )
    {
    #if defined (HAS_RECURSIVE_MUTEXES)
        (void)ADAPT_RETVAL( pthread_mutexattr_settype(args, iLockType), result );
    #endif
    }
    
    if( 0 == result )
    {
        result = ADAPT_RETVAL( pthread_mutex_init(&this->m_Lock, args), result );
    }
    
    if( args == &attrTemp && iAttrInit )
    {
        pthread_mutexattr_destroy( &attrTemp );
    }
    
    cout<<"RecursiveThreadMutex Result="<<result<<endl;
}

RecursiveThreadMutex::~RecursiveThreadMutex( void )
{
    this->Remove();
}

int RecursiveThreadMutex::Remove( void )
{
    int result = 0;
    
    if( !this->m_IsRemoved )
    {
        this->m_IsRemoved = true;
        
        result = ADAPT_RETVAL( pthread_mutex_destroy(&this->m_Lock), result );
    }
    
    return result;
}

int RecursiveThreadMutex::Acquire( void )
{
    int result = 0;
    return ADAPT_RETVAL( pthread_mutex_lock(&this->m_Lock), result );
}

int RecursiveThreadMutex::TryAcquire( void )
{
    int result = 0;
    return ADAPT_RETVAL( pthread_mutex_trylock(&this->m_Lock), result );
}

int RecursiveThreadMutex::Release( void )
{
    int result = 0;
    return ADAPT_RETVAL( pthread_mutex_unlock(&this->m_Lock), result );
}

recursive_thread_mutext_t& RecursiveThreadMutex::GetLock( void )
{
    return this->m_Lock;
}

thread_mutex_t& RecursiveThreadMutex::GetNestingMutex( void )
{
    return static_cast<thread_mutex_t &>(this->m_Lock);
}

typedef CleanupAdapter<RecursiveThreadMutex> StaticObjectLockType;
static StaticObjectLockType *g_StaticObjectLock = NULL;

RecursiveThreadMutex* StaticObjectLock::GetInstance( void )
{
    if( NULL == g_StaticObjectLock )
    {
        NEW_RETURN( g_StaticObjectLock, StaticObjectLockType, 0 );
    }

    return &( g_StaticObjectLock->GetObject() );
}


