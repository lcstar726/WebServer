#ifndef __SYNCH_GUARD_H__
#define __SYNCH_GUARD_H__

template<class LOCK>
class SynchGuard
{
protected:
    SynchGuard( LOCK *lock ): m_Lock(lock), m_Owner( 0 ) { }
public:
    SynchGuard( LOCK &lock );
    
    SynchGuard( LOCK &lock, bool isBlock );
    
    virtual ~SynchGuard( void );
    
    virtual int Acquire( void );
    
    virtual int TryAcquire( void );
    
    virtual int Release( void );
    
    virtual int Remove( void );
    
    bool IsLocked( void ) const;
    
protected:
    int m_Owner;
    LOCK *m_Lock;

private:
    SynchGuard( const SynchGuard<LOCK>& );
    void operator= ( const SynchGuard<LOCK>& );
};

template<class LOCK>
inline SynchGuard<LOCK>::SynchGuard( LOCK &lock ): m_Lock( &lock ), m_Owner( 0 )
{
    this->Acquire();
}

template<class LOCK>
inline SynchGuard<LOCK>::SynchGuard( LOCK &lock, bool isBlock ): m_Lock( &lock ), m_Owner( 0 )
{
    if( isBlock )
    {
        this->Acquire();
    }
    else
    {
        this->TryAcquire();
    }
}

template<class LOCK>
inline SynchGuard<LOCK>::~SynchGuard()
{
    this->Release();
}

template<class LOCK>
inline int SynchGuard<LOCK>::Acquire( void )
{
    return this->m_Owner = this->m_Lock->Acquire();
}

template<class LOCK>
inline int SynchGuard<LOCK>::TryAcquire( void )
{
    return this->m_Owner = this->m_Lock->TryAcquire();
}

template<class LOCK>
inline int SynchGuard<LOCK>::Release( void )
{
    if( -1 == m_Owner )
    {
        return -1;
    }
    else
    {
        m_Owner = -1;
        return this->m_Lock->Release();
    }
}

template<class LOCK>
inline int SynchGuard<LOCK>::Remove( void )
{
    return this->m_Lock->Remove();
}

template<class LOCK>
inline bool SynchGuard<LOCK>::IsLocked( void ) const
{
    return ( this->m_Owner != -1 );
}

template<class LOCK>
class ReadGuard: public SynchGuard<LOCK>
{
public:
    ReadGuard( LOCK &lock );
    
    ReadGuard( LOCK &lock, bool isBlock );
    
    int Acquire( void );
    int AcquireRead( void );
    
    int TryAcquire( void );
    int TryAcquireRead( void );
};

template<class LOCK>
ReadGuard<LOCK>::ReadGuard( LOCK &lock ): SynchGuard<LOCK>( &lock )
{
    this->AcquireRead();
}

template<class LOCK>
ReadGuard<LOCK>::ReadGuard( LOCK &lock, bool isBlock ): SynchGuard<LOCK>( &lock )
{
    if( isBlock )
    {
        this->AcquireRead();
    }
    else
    {
        this->TryAcquireRead();
    }
}

template<class LOCK>
inline int ReadGuard<LOCK>::Acquire( void )
{
    return this->m_Owner = this->m_Lock->AcquireRead();
}
template<class LOCK>
inline int ReadGuard<LOCK>::AcquireRead( void )
{
    return this->m_Owner = this->m_Lock->AcquireRead();
}

template<class LOCK>
inline int ReadGuard<LOCK>::TryAcquire()
{
    return this->m_Owner = this->m_Lock->TryAcquireRead();
}
template<class LOCK>
inline int ReadGuard<LOCK>::TryAcquireRead()
{
    return this->m_Owner = this->m_Lock->TryAcquireRead();
}

#endif
