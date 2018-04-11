#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "Public.h"
#include "TimeValue.h"

typedef struct
{
    sem_t   *m_pSemHandle;
    char    *m_cstrName;
    mutex_t m_Lock;
    cond_t  m_CountNonZero;
}tSEMAPHORE;

class Semaphore
{
public:
    Semaphore( uint iCount = 1
            , int iType = USYNC_THREAD
            , const char *cstrName = NULL
            , void *arg = NULL
            , int iMax = 0x7ffffffff );
    ~Semaphore( void );

    int Acquire( void );
    int Acquire( TimeValue &timeOut );
    int Acquire( TimeValue *timeOut );
    int TryAcquire( void );

    int Release( void );
    int Release( uint iReleaseCount );

    int Remove( void );

    const tSEMAPHORE& GetLock( void ) const;

protected:
    tSEMAPHORE  m_Semaphore;
    bool        m_bIsRemoved;

private:
    Semaphore( const Semaphore& );
    void operator =( const Semaphore& );
};

namespace PubFunc
{
    int SemaphoreInit( tSEMAPHORE *sem
                       , uint count
                       , int type = USYNC_THREAD
                       , const char *name = 0
                       , void *arg = 0
                        , int max = 0x7fffffff );
    
    int SemaphoreInit( tSEMAPHORE *sem
                       , uint count
                       , int type
                       , condattr_t *attributes
                       , const char *name = 0
                       , void *arg = 0
                       , int max = 0x7fffffff );

    int SemaphoreDestroy( tSEMAPHORE *sem );

    int SemaphoreWait( tSEMAPHORE *sem);
    int SemaphoreWait( tSEMAPHORE *sem, TimeValue &tv );
    int SemaphoreWait( tSEMAPHORE *sem, TimeValue *tv );
    
    int SemaphoreTryWait( tSEMAPHORE *sem );

    int SemaphorePost( tSEMAPHORE *sem );
    int SemaphorePost( tSEMAPHORE *sem, uint iReleaseCount );

};

#endif

