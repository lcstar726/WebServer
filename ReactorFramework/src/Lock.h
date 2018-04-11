#ifndef __LOCK_H__
#define __LOCK_H__

#include "Public.h"
#include "errno.h"

class Lock
{
public:
    Lock( void );
    
    virtual ~Lock( void );
    
    virtual int Acquire( void ) = 0;
    virtual int TryAcquire( void ) = 0;
    
    virtual int Release( void ) = 0;
    
    virtual int Remove( void ) = 0;
};

template<class LOCKING_MECHANISM>
class LockAdapter: public Lock
{
public:
    typedef LOCKING_MECHANISM LOCK;
    
    LockAdapter( void );
    
    LockAdapter( LOCKING_MECHANISM &lock );
    
    virtual ~LockAdapter( void );
    
    virtual int Acquire( void );
    virtual int TryAcquire( void );
    
    virtual int Release( void );
    
    virtual int Remove( void );
    
private:
    LOCKING_MECHANISM *m_pLock;
    
    bool m_bNeedDelete;
};

template<class LOCKING_MECHANISM>
inline LockAdapter<LOCKING_MECHANISM>::LockAdapter( void )
    : m_pLock( NULL ), m_bNeedDelete( true )
{
    NEW( this->m_pLock, LOCKING_MECHANISM );
        
    if( NULL == this->m_pLock )
    {
        errno = ENOMEM;
    }
}

template<class LOCKING_MECHANISM>
inline LockAdapter<LOCKING_MECHANISM>::LockAdapter( LOCKING_MECHANISM &lock )
    : m_pLock( &lock ), m_bNeedDelete( false )
{
}

template<class LOCKING_MECHANISM>
LockAdapter<LOCKING_MECHANISM>::~LockAdapter( void )
{
    if( this->m_bNeedDelete )
    {
        delete this->m_pLock;
    }
}

template<class LOCKING_MECHANISM>
int LockAdapter<LOCKING_MECHANISM>::Acquire( void )
{
    return this->m_pLock->Acquire();
}

template<class LOCKING_MECHANISM>
int LockAdapter<LOCKING_MECHANISM>::TryAcquire( void )
{
    return this->m_pLock->TryAcquire();
}

template<class LOCKING_MECHANISM>
int LockAdapter<LOCKING_MECHANISM>::Release( void )
{
    return this->m_pLock->Release();
}

template<class LOCKING_MECHANISM>
int LockAdapter<LOCKING_MECHANISM>::Remove( void )
{
    return this->m_pLock->Remove();
}

namespace AcquireMethod
{
    enum METHOD_TYPE
    {
        REGULAR
        , READ
        , WRITE
    };
}

template<class LOCKING_MECHANISM>
class ReverseLock: public Lock
{
public:
    typedef LOCKING_MECHANISM LOCK;

    ReverseLock( LOCK &lock, AcquireMethod::METHOD_TYPE acquireMethod = AcquireMethod::REGULAR )
        : m_Lock( lock ), m_AcquireMethod( acquireMethod )
    {
    }
    virtual ~ReverseLock( void );

    virtual int Acquire( void );
    virtual int TryAcquire( void );
    virtual int Release( void );
    virtual int Remove( void );

private:
    LOCKING_MECHANISM&          m_Lock;
    AcquireMethod::METHOD_TYPE  m_AcquireMethod;
};

template<class LOCKING_MECHANISM>
ReverseLock<LOCKING_MECHANISM>::~ReverseLock( void )
{
}

template<class LOCKING_MECHANISM>
int ReverseLock<LOCKING_MECHANISM>::Acquire( void )
{
    return this->m_Lock.Release();
}
template<class LOCKING_MECHANISM>
int ReverseLock<LOCKING_MECHANISM>::TryAcquire( void )
{
    return -1;
}

template<class LOCKING_MECHANISM>
int ReverseLock<LOCKING_MECHANISM>::Release( void )
{
    if( AcquireMethod::READ == this->m_AcquireMethod )
    {
        return this->m_Lock.Acquire();
    }
    else if( AcquireMethod::WRITE == this->m_AcquireMethod )
    {
        return this->m_Lock.Acquire();
    }
    else
    {
        return this->m_Lock.Acquire();
    }
}

template<class LOCKING_MECHANISM>
int ReverseLock<LOCKING_MECHANISM>::Remove( void )
{
    return this->m_Lock.Remove();
}

#endif

