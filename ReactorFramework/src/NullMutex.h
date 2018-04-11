#ifndef __NULL_MUTEX_H__
#define __NULL_MUTEX_H__

class NullMutex
{
public:
    NullMutex( const char* = 0 ): m_Lock(0) { }
    ~NullMutex( void ) { }

    int Remove( void )
    {
        return 0;
    }
    
    int Acquire( void )
    {
        return 0;
    }
    
    int TryAcquire( void )
    {
        return 0;
    }
    
    int AcquireWrite( void )
    {
        return 0;
    }
    
    int TryAcquireWrite( void )
    {
        return 0;
    }
    
    int AcquireRead( void )
    {
        return 0;
    }
    
    int TryAcquireRead( void )
    {
        return 0;
    }
    
    int Release( void )
    {
        return 0;
    }

    int m_Lock;
};

template<class LOCK>
class NormalGuard;

template<>
class NormalGuard<NullMutex>
{
public:
    NormalGuard( NullMutex & ){ }
    NormalGuard( NullMutex &, int ) { }
    NormalGuard( NullMutex &, int, int ) { }
    
    int Acquire( void )
    {
        return 0;
    }
    
    int TryAcquire( void )
    {
        return 0;
    }
    
    int Release( void )
    {
        return 0;
    }
    
    void Disown( void ){ }
    
    int Locked( void )
    {
        return 1;
    }
    
    int Remove( void )
    {
        return 0;
    }
    
private:
    NormalGuard( const NormalGuard<NullMutex> & );
    void operator= ( const NormalGuard<NullMutex> & );
};

#endif
