#ifndef __ATOMIC_OP_H__
#define __ATOMIC_OP_H__

#include "Public.h"
#include "TypeTraits.h"
#include "SynchTraits.h"
#include "ThreadMutex.h"

template<typename TYPE>
class AtomicOpGcc
{
public:
    TYPE operator ++( void );
    TYPE operator ++( int );
    TYPE operator +=( TYPE rhs );

    TYPE operator --( void );
    TYPE operator --( int );
    TYPE operator -=( TYPE rhs );

    TYPE Exchange( TYPE newValue );

    TYPE GetValue( void ) const;
    volatile TYPE& GetValueRef( void );

protected:
    AtomicOpGcc( void );
    AtomicOpGcc( TYPE obj );
    AtomicOpGcc( AtomicOpGcc<TYPE> const &obj );

    AtomicOpGcc<TYPE>& operator =( TYPE rhs );
    AtomicOpGcc<TYPE>& operator =( AtomicOpGcc<TYPE> const &rhs );

private:
    ThreadMutex& GetMutex( void );

    volatile TYPE m_Value;
};

template<typename TYPE>
AtomicOpGcc<TYPE>::AtomicOpGcc( void )
    : m_Value( 0 )
{
}
template<typename TYPE>
AtomicOpGcc<TYPE>::AtomicOpGcc( TYPE obj )
    : m_Value( obj )
{
}
template<typename TYPE>
AtomicOpGcc<TYPE>::AtomicOpGcc( AtomicOpGcc<TYPE> const &obj )
    : m_Value( obj.m_Value )
{
}

template<typename TYPE>
inline AtomicOpGcc<TYPE>& AtomicOpGcc<TYPE>::operator =( TYPE rhs )
{
    (void) __sync_lock_test_and_set( &this->m_Value, rhs );

    return *this;
}
template<typename TYPE>
inline AtomicOpGcc<TYPE>& AtomicOpGcc<TYPE>::operator =( AtomicOpGcc<TYPE> const &rhs )
{
    (void) __sync_lock_test_and_set( &this->m_Value, rhs.m_Value );

    return *this;
}

template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator ++( void )
{
    return __sync_add_and_fetch( &this->m_Value, 1 );
}
template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator ++( int )
{
    return __sync_fetch_and_add( &this->m_Value, 1 );
}
template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator +=( TYPE rhs )
{
    return __sync_add_and_fetch( &this->m_Value, rhs );
}

template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator --( void )
{
    return __sync_sub_and_fetch( &this->m_Value, 1 );
}
template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator --( int )
{
    return __sync_fetch_and_sub( &this->m_Value, 1 );
}
template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::operator -=( TYPE rhs )
{
    return __sync_sub_and_fetch( &this->m_Value, rhs );
}

template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::Exchange( TYPE newValue )
{
    return __sync_val_compare_and_swap( &this->m_Value, this->m_Value, newValue );
}

template<typename TYPE>
inline TYPE AtomicOpGcc<TYPE>::GetValue( void ) const
{
    return this->m_Value;
}
template<typename TYPE>
inline volatile TYPE& AtomicOpGcc<TYPE>::GetValueRef( void )
{
    return this->m_Value;
}

template<class LOCK, typename TYPE>
class AtomicOpEx
{
public:
    typedef typename Loki::TypeTraits<TYPE>::ParameterType ArgType;

    AtomicOpEx( LOCK &mutex );
    AtomicOpEx( LOCK &mutex, ArgType arg );
    AtomicOpEx( const AtomicOpEx<LOCK, TYPE> &obj );

    AtomicOpEx<LOCK, TYPE>& operator =( ArgType rhs );
    AtomicOpEx<LOCK, TYPE>& operator =( AtomicOpEx<LOCK, TYPE> const &rhs );

    TYPE operator ++( void );
    TYPE operator ++( int );
    TYPE operator +=( ArgType rhs );

    TYPE operator --( void );
    TYPE operator --( int );
    TYPE operator -=( ArgType rhs );

    LOCK& GetMutex( void );
    TYPE GetValue( void ) const;
    TYPE& GetValueRef( void );

private:
    LOCK    &m_Mutex;
    TYPE    m_Value;
};

template<class LOCK, typename TYPE>
AtomicOpEx<LOCK, TYPE>::AtomicOpEx( LOCK &mutex )
    : m_Mutex( mutex ), m_Value( 0 )
{
}
template<class LOCK, typename TYPE>
AtomicOpEx<LOCK, TYPE>::AtomicOpEx( LOCK &mutex, typename AtomicOpEx<LOCK,TYPE>::ArgType arg )
    : m_Mutex( mutex ), m_Value( arg )
{
}
template<class LOCK, typename TYPE>
inline AtomicOpEx<LOCK, TYPE>::AtomicOpEx( const AtomicOpEx<LOCK, TYPE> &obj )
    : m_Mutex( obj.m_Mutex ), m_Value( obj.m_Value )
{
}

template<class LOCK, typename TYPE>
inline AtomicOpEx<LOCK, TYPE>& AtomicOpEx<LOCK, TYPE>::operator =( typename AtomicOpEx<LOCK, TYPE>::ArgType rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, *this );

    this->m_Value = rhs;
    
    return *this;
}
template<class LOCK, typename TYPE>
inline AtomicOpEx<LOCK, TYPE>& AtomicOpEx<LOCK, TYPE>::operator =( AtomicOpEx<LOCK, TYPE> const &rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, *this );

    this->m_Value = rhs.m_Value;

    return *this;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator ++( void )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return ++this->m_Value;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator ++( int )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return this->m_Value++;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator +=( typename AtomicOpEx<LOCK, TYPE>::ArgType rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return this->m_Value += rhs;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator --( void )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return --this->m_Value;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator --( int )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return this->m_Value--;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::operator -=( typename AtomicOpEx<LOCK, TYPE>::ArgType rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return this->m_Value -= rhs;
}

template<class LOCK, typename TYPE>
LOCK& AtomicOpEx<LOCK, TYPE>::GetMutex( void )
{
    return this->m_Mutex;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOpEx<LOCK, TYPE>::GetValue( void ) const
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, this->m_Value );

    return this->m_Value;
}

template<class LOCK, typename TYPE>
inline TYPE& AtomicOpEx<LOCK, TYPE>::GetValueRef( void )
{
    return this->m_Value;
}

template<class LOCK, typename TYPE>
class AtomicOp
{
public:
    typedef typename Loki::TypeTraits<TYPE>::ParameterType ArgType;

    AtomicOp( void );
    AtomicOp( ArgType arg );
    AtomicOp( AtomicOp<LOCK, TYPE> const &obj );

    AtomicOp<LOCK, TYPE>& operator= ( ArgType rhs );
    AtomicOp<LOCK, TYPE>& operator= ( AtomicOp<LOCK, TYPE> const & rhs );

    TYPE operator++ ( void );
    TYPE operator++ ( int );
    TYPE operator+= ( ArgType rhs );

    TYPE operator-- ( void );
    TYPE operator-- ( int );
    TYPE operator-= ( ArgType rhs );

    TYPE GetValue( void ) const;
    TYPE& GetValueRef( void );

private:
    LOCK m_OwnMutex;

    AtomicOpEx<LOCK, TYPE>  m_AtomicOpImpl;
};

template<class LOCK, typename TYPE>
AtomicOp<LOCK, TYPE>::AtomicOp( void )
    : m_AtomicOpImpl( this->m_OwnMutex )
{
}
template<class LOCK, typename TYPE>
AtomicOp<LOCK, TYPE>::AtomicOp( typename AtomicOp<LOCK, TYPE>::ArgType arg )
    : m_AtomicOpImpl( this->m_OwnMutex, arg )
{
}
template<class LOCK, typename TYPE>
AtomicOp<LOCK, TYPE>::AtomicOp( AtomicOp<LOCK, TYPE> const &obj )
    : m_AtomicOpImpl( this->m_OwnMutex, obj.GetValue() )
{
}

template<class LOCK, typename TYPE>
AtomicOp<LOCK, TYPE>& AtomicOp<LOCK, TYPE>::operator =( typename AtomicOp<LOCK, TYPE>::ArgType rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_OwnMutex, *this );

    this->m_AtomicOpImpl = rhs;

    return *this;
}
template<class LOCK, typename TYPE>
AtomicOp<LOCK, TYPE>& AtomicOp<LOCK, TYPE>::operator =( AtomicOp<LOCK, TYPE> const &rhs )
{
    GUARD_RETURN( LOCK, monitor, this->m_OwnMutex, *this );

    this->m_AtomicOpImpl = rhs.m_AtomicOpImpl;

    return *this;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator ++( void )
{
    return ++this->m_AtomicOpImpl;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator ++( int )
{
    return this->m_AtomicOpImpl++;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator +=( typename AtomicOp<LOCK, TYPE>::ArgType rhs )
{
    return this->m_AtomicOp += rhs;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator --( void )
{
    return --this->m_AtomicOpImpl;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator --( int )
{
    return this->m_AtomicOpImpl--;
}
template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::operator -=( typename AtomicOp<LOCK, TYPE>::ArgType rhs )
{
    this->m_AtomicOpImpl -= rhs;
}

template<class LOCK, typename TYPE>
inline TYPE AtomicOp<LOCK, TYPE>::GetValue( void ) const
{
    return this->m_AtomicOpImpl.GetValue();
}
template<class LOCK, typename TYPE>
inline TYPE& AtomicOp<LOCK, TYPE>::GetValueRef( void )
{
    return this->m_AtomicOpImpl.GetValueRef();
}

template<>
class AtomicOp<ThreadMutex, int>: public AtomicOpGcc<int>
{
public:
    AtomicOp( void );
    AtomicOp( int iValue );
    AtomicOp( AtomicOp<ThreadMutex, int> const &obj );

    AtomicOp<ThreadMutex, int>& operator =( int iValue );
};
inline AtomicOp<ThreadMutex, int>::AtomicOp( void )
    : AtomicOpGcc<int>()
{
}
inline AtomicOp<ThreadMutex, int>::AtomicOp( int iValue )
    : AtomicOpGcc<int>( iValue )
{
}
inline AtomicOp<ThreadMutex, int>::AtomicOp( AtomicOp<ThreadMutex, int> const &obj )
    : AtomicOpGcc<int>( obj )
{
}
inline AtomicOp<ThreadMutex, int>& AtomicOp<ThreadMutex, int>::operator =( int iValue )
{
    AtomicOpGcc<int>::operator =( iValue );

    return *this;
}

template<>
class AtomicOp<ThreadMutex, long>: public AtomicOpGcc<long>
{
public:
    AtomicOp( void );
    AtomicOp( long lValue );
    AtomicOp( AtomicOp<ThreadMutex, long> const &obj );

    AtomicOp<ThreadMutex, long>& operator =( long lValue );
};
inline AtomicOp<ThreadMutex, long>::AtomicOp( void )
    : AtomicOpGcc<long>()
{
}
inline AtomicOp<ThreadMutex, long>::AtomicOp( long lValue )
    : AtomicOpGcc<long>( lValue )
{
}
inline AtomicOp<ThreadMutex, long>::AtomicOp( AtomicOp<ThreadMutex, long> const &obj )
    : AtomicOpGcc<long>( obj )
{
}
inline AtomicOp<ThreadMutex, long>& AtomicOp<ThreadMutex, long>::operator =( long lValue )
{
    AtomicOpGcc<long>::operator =( lValue );

    return *this;
}

#endif

