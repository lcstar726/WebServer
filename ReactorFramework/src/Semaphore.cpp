#include "Semaphore.h"

int PubFunc::SemaphoreInit( tSEMAPHORE *sem
                          , uint count
                          , int type
                          , const char *name
                          , void *arg
                          , int max)
{
    condattr_t *pattr = NULL;
    return PubFunc::SemaphoreInit(sem, count, type, pattr, name, arg, max);
}
int PubFunc::SemaphoreInit( tSEMAPHORE *sem
                           , uint count
                           , int type
                           , condattr_t *attributes
                           , const char *name
                           , void *arg
                           , int max)
{
    sem->m_cstrName = NULL;

    if(name)
    {
        ALLOCATOR_RETURN(sem->m_cstrName, strdup(name), -1);
        sem->m_pSemHandle = sem_open(sem->m_cstrName
                                    , O_CREAT
                                    , DEFAULT_FILE_PERMS
                                    , count);
        if(sem->m_pSemHandle == (sem_t *) SEM_FAILED)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        NEW_RETURN(sem->m_pSemHandle, sem_t, -1);
        OSCALL_RETURN(sem_init(sem->m_pSemHandle, type != USYNC_THREAD, count), int, -1);
    }
}

int PubFunc::SemaphoreDestroy(tSEMAPHORE *sem)
{
    int iResult;

    if(sem->m_cstrName)
    {
        OSCALL(sem_unlink(sem->m_cstrName), int, -1, iResult);
        free((void *) sem->m_cstrName);
        OSCALL_RETURN(sem_close(sem->m_pSemHandle), int, -1);
    }
    else
    {
        OSCALL(sem_destroy(sem->m_pSemHandle), int, -1, iResult);
        delete sem->m_pSemHandle;
        sem->m_pSemHandle = 0;
        return iResult;
    }
}

int PubFunc::SemaphoreWait(tSEMAPHORE *sem)
{
    OSCALL_RETURN(sem_wait(sem->m_pSemHandle), int, -1);
}

int PubFunc::SemaphoreWait(tSEMAPHORE *sem, TimeValue &tv)
{
    int rc;
    timespec_t ts;
    ts = tv; // Calls ACE_Time_Value::operator timespec_t().
    OSCALL( sem_timedwait( sem->m_pSemHandle, &ts), int, -1, rc );
    if( rc == -1 && errno == ETIMEDOUT )
    {
        errno = ETIME;  /* POSIX returns ETIMEDOUT but we need ETIME */
    }
    return rc;
}

int PubFunc::SemaphoreWait(tSEMAPHORE *sem, TimeValue *tv)
{
    return NULL == tv ? SemaphoreWait( sem ) : SemaphoreWait( sem, *tv );
}

int PubFunc::SemaphoreTryWait( tSEMAPHORE *sem )
{
    OSCALL_RETURN( sem_trywait(sem->m_pSemHandle), int, -1 );
}

int PubFunc::SemaphorePost( tSEMAPHORE *sem )
{
    OSCALL_RETURN( sem_post(sem->m_pSemHandle), int, -1 );
}
int PubFunc::SemaphorePost( tSEMAPHORE *sem, uint iReleaseCount )
{
    for( size_t i = 0; i < iReleaseCount; ++i )
    {
        if ( -1 == sem_post(sem->m_pSemHandle) )
        {
            return -1;
        }
    }
    return 0;
}

Semaphore::Semaphore(uint iCount
                     , int iType
                     , const char *cstrName
                     , void *arg
                     , int iMax)
    : m_bIsRemoved(false)
{
    if(PubFunc::SemaphoreInit(&this->m_Semaphore, iCount, iType, cstrName, arg, iMax) != 0)
    {
        cout << __FILE__ << "_" << __LINE__ << "__Semaphore::Semaphore ctr failed!" << endl;
    }
    cout << __FILE__ << "_" << __LINE__ << "__Semaphore::Semaphore ctr success!" << endl;
}
Semaphore::~Semaphore(void)
{
    this->Remove();
}

int Semaphore::Acquire(void)
{
    return PubFunc::SemaphoreWait( &this->m_Semaphore );
}
int Semaphore::Acquire(TimeValue &timeOut)
{
    return PubFunc::SemaphoreWait( &this->m_Semaphore, timeOut );
}
int Semaphore::Acquire(TimeValue *timeOut)
{
    return PubFunc::SemaphoreWait( &this->m_Semaphore, *timeOut );
}
int Semaphore::TryAcquire(void)
{
    return PubFunc::SemaphoreTryWait( &this->m_Semaphore );
}

int Semaphore::Release( void )
{
    return PubFunc::SemaphorePost( &this->m_Semaphore );
}
int Semaphore::Release( uint iReleaseCount )
{
    return PubFunc::SemaphorePost( &this->m_Semaphore, iReleaseCount );
}

int Semaphore::Remove( void )
{
    int iRet = 0;
    if( !this->m_bIsRemoved )
    {
        this->m_bIsRemoved = true;
        iRet = PubFunc::SemaphoreDestroy( &this->m_Semaphore );
    }
    return iRet;
}

const tSEMAPHORE& Semaphore::GetLock(void) const
{
    return this->m_Semaphore;
}

