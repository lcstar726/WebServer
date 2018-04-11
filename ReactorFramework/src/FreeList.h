#ifndef __FREE_LIST_H__
#define __FREE_LIST_H__

#include "Public.h"
#include "SynchTraits.h"

#define FREE_LIST_WITH_POOL 1
#define PURE_FREE_LIST 2


#if !defined (DEFAULT_FREE_LIST_PREALLOC)
#define DEFAULT_FREE_LIST_PREALLOC 0
#endif

#if !defined (DEFAULT_FREE_LIST_LWM)
#define DEFAULT_FREE_LIST_LWM 0
#endif

#if !defined (DEFAULT_FREE_LIST_HWM)
#define DEFAULT_FREE_LIST_HWM 25000
#endif

#if !defined (DEFAULT_FREE_LIST_INC)
#define DEFAULT_FREE_LIST_INC 100
#endif

template<class T>
class FreeList
{
public:
    virtual ~FreeList( void ) { }
    
    virtual void Add( T *element ) = 0;
    
    virtual T* Remove( void ) = 0;
    
    virtual size_t GetSize( void ) = 0;
    
    virtual void Resize( size_t newSize ) = 0;
};

template<class T, class LOCK>
class LockedFreeList: public FreeList<T>
{
public:
    LockedFreeList( int mode = FREE_LIST_WITH_POOL
                    , size_t preAlloc = DEFAULT_FREE_LIST_PREALLOC
                    , size_t lowWM = DEFAULT_FREE_LIST_LWM
                    , size_t highWM = DEFAULT_FREE_LIST_HWM
                    , size_t inc = DEFAULT_FREE_LIST_INC );

    virtual ~LockedFreeList( void );
    
    virtual void Add( T *element );
    
    virtual T* Remove( void );
    
    virtual size_t GetSize( void );
    
    virtual void Resize( size_t newSize );

protected:
    virtual void Alloc( size_t cnt );
    
    virtual void Dealloc( size_t cnt );

    int m_Mode;

    T* m_FreeList;

    size_t m_LowWM;
    size_t m_HighWM;
    size_t m_Increment;

    size_t m_Size;

    LOCK m_Mutex;
};

template<class T, class LOCK>
LockedFreeList<T, LOCK>::LockedFreeList( int mode
                    , size_t preAlloc
                    , size_t lowWM
                    , size_t highWM
                    , size_t inc )
    :m_Mode( mode ), m_FreeList( 0 ), m_LowWM( lowWM ), m_HighWM( highWM ), m_Increment( inc )
{
    this->Alloc( preAlloc );
}

template<class T, class LOCK>
LockedFreeList<T, LOCK>::~LockedFreeList( void )
{
    if( this->m_Mode != PURE_FREE_LIST )
    {
        while( this->m_FreeList != 0 )
        {
            T *temp = this->m_FreeList;
            this->m_FreeList = this->m_FreeList->GetNext();
            delete temp;
        }
    }
}

template<class T, class LOCK>
void LockedFreeList<T, LOCK>::Add( T *element )
{
    SYNCH_GUARD( LOCK, monitor, this->m_Mutex );

    if( PURE_FREE_LIST == this->m_Mode || this->m_Size < this->m_HighWM )
    {
        element->SetNext( this->m_FreeList );
        this->m_FreeList = element;
        ++this->m_Size;
    }
    else
    {
        delete element;
    }
}

template<class T, class LOCK>
T* LockedFreeList<T, LOCK>::Remove( void )
{
    GUARD_RETURN( LOCK, monitor, this->m_Mutex, 0 );

    if( this->m_Size <= this->m_LowWM )
    {
        this->Alloc( this->m_Increment );
    }

    T *element = this->m_FreeList;
    if( element != 0 )
    {
        this->m_FreeList = this->m_FreeList->GetNext();
        --this->m_Size;
    }

    return element;
}

template<class T, class LOCK>
size_t LockedFreeList<T, LOCK>::GetSize()
{
    return this->m_Size;
}

template<class T, class LOCK>
void LockedFreeList<T,LOCK>::Resize( size_t newSize )
{
    SYNCH_GUARD( LOCK, monitor, this->m_Mutex );

    if( this->m_Mode != PURE_FREE_LIST )
    {
        if( newSize < this->m_Size )
        {
            this->Dealloc( this->m_Size - newSize );
        }
        else if( newSize > this->m_Size )
        {
            this->Alloc( newSize - this->m_Size );
        }
    }
}

template<class T, class LOCK>
void LockedFreeList<T,LOCK>::Alloc( size_t cnt )
{
    for( ; cnt > 0; --cnt )
    {
        T *element = 0;
        NEW( element, T );
        element->SetNext( this->m_FreeList );
        this->m_FreeList = element;
        ++this->m_Size;
    }
}

template<class T, class LOCK>
void LockedFreeList<T, LOCK>::Dealloc( size_t cnt )
{
    for( ; cnt > 0; --cnt )
    {
        T *element = this->m_FreeList;
        this->m_FreeList = this->m_FreeList->GetNext();
        delete element;
        --this->m_Size;
    }
}

#endif
