#ifndef __CONTAINERS_H__
#define __CONTAINERS_H__

#include "GlobalMacros.h"
#include "Allocator.h"

template<class T>
class DoubleLinkedList;

template<class T>
class DoubleLinkedListIteratorBase
{
public:
    T* NotDone( void ) const;

    int Done( void ) const;

    T* GetCurrent( void ) const;

    T& operator * (void) const;

    T* DoAdvance( void );
    T* DoRetreat( void );

protected:
    DoubleLinkedListIteratorBase( const DoubleLinkedList<T> & );

    DoubleLinkedListIteratorBase( const DoubleLinkedListIteratorBase<T> &iterator );

    const DoubleLinkedList<T> *m_DLList;

    T *m_Current;
};

template<class T>
class DoubleLinkedListIterator: public DoubleLinkedListIteratorBase<T>
{
public:
    DoubleLinkedListIterator( const DoubleLinkedList<T> & );

    DoubleLinkedListIterator<T>& operator++ ( void );
    DoubleLinkedListIterator<T>& operator++ ( int );
    DoubleLinkedListIterator<T>& operator-- ( void );
    DoubleLinkedListIterator<T>& operator-- ( int );
};

template<class T>
class DoubleLinkedListReverseIterator:public DoubleLinkedListIteratorBase<T>
{
public:
    DoubleLinkedListReverseIterator( const DoubleLinkedList<T> & );

    DoubleLinkedListReverseIterator<T>& operator ++( void );
    DoubleLinkedListReverseIterator<T>& operator ++( int );
    DoubleLinkedListReverseIterator<T>& operator --( void );
    DoubleLinkedListReverseIterator<T>& operator --( int );
};

template<class T>
class DoubleLinkedList
{
    friend class DoubleLinkedListIteratorBase<T>;
    friend class DoubleLinkedListIterator<T>;
    friend class DoubleLinkedListReverseIterator<T>;
public:
    typedef DoubleLinkedListIterator<T> ITERATOR;
    typedef DoubleLinkedListReverseIterator<T> REVERSE_ITERATOR;

    DoubleLinkedList( AllocatorBase *alloc = 0 );
    DoubleLinkedList( const DoubleLinkedList<T> & );

    ~DoubleLinkedList( void );

    void operator =( const DoubleLinkedList<T> & );

    int IsEmpty( void ) const;

    T* InsertHead( T* newItem );
    T* InsertTail( T* newItem );

    T* DeleteHead( void );
    T* DeleteTail( void );

    int GetNode( T *&item, int slot );

protected:
    void InitHead( void );

    int InsertElement( T *newItem, int before = 0, T *oldItem = 0 );

    int RemoveElement( T *oldItem );

    void CopyNodes( const DoubleLinkedList<T> &rhs );

    void DeleteNodes( void );

    T *m_Head;

    size_t m_Size;

    AllocatorBase *m_Allocator;
};

class DoubleLinkedListNode
{
    friend class DoubleLinkedList<DoubleLinkedListNode>;
    friend class DoubleLinkedListIteratorBase<DoubleLinkedListNode>;
    friend class DoubleLinkedListIterator<DoubleLinkedListNode>;
    friend class DoubleLinkedListReverseIterator<DoubleLinkedListNode>;

public:
    DoubleLinkedListNode( void *item
                        , DoubleLinkedListNode *next = 0
                        , DoubleLinkedListNode *prev = 0 )
            :m_Item( item ), m_Next( next ), m_Prev( prev ) {}
                        
    void *m_Item;
    DoubleLinkedListNode *m_Next;
    DoubleLinkedListNode *m_Prev;

protected:
    DoubleLinkedListNode( void ):m_Item( 0 ), m_Next( 0 ), m_Prev( 0 ) {}
};

typedef DoubleLinkedList<DoubleLinkedListNode> DLListBase;

template<class T>
class DLList: public DLListBase
{
public:
    DLList( AllocatorBase *alloc = 0 );
    DLList( const DLList<T> &list );
    ~DLList( void );

    T* InsertHead( T *newItem );
    T* InsertTail( T *newItem );

    T* DeleteHead( void );
    T* DeleteTail( void );

    int GetItem( T *&item, int slot = 0 );

    //int Remove( DoubleLinkedListNode *node );
};

template<class T>
class DLListIterator: public DoubleLinkedListIterator<DoubleLinkedListNode>
{
    friend class DLList<T>;
};

template<class T>
inline DLList<T>::DLList( AllocatorBase *alloc )
    :DLListBase( alloc )
{
}

template<class T>
inline DLList<T>::DLList( const DLList<T> &list )
    :DLListBase( list )
{
}

template<class T>
inline DLList<T>::~DLList( void )
{
    while( this->DeleteHead() );
}

template<class T>
T* DLList<T>::InsertHead( T *newItem )
{
    DoubleLinkedListNode *tmp1 = 0;
    NEW_MALLOC_RETURN( tmp1
                    , (DoubleLinkedListNode*)this->m_Allocator->Malloc(sizeof(DoubleLinkedListNode))
                    , DoubleLinkedListNode( newItem )
                    , 0 );

    DoubleLinkedListNode *tmp2 = DLListBase::InsertHead( tmp1 );

    return (T*)( tmp2 ? tmp2->m_Item : 0 );
}

template<class T>
T* DLList<T>::InsertTail( T *newItem )
{
    DoubleLinkedListNode *tmp1 = 0;
    NEW_MALLOC_RETURN( tmp1
                    , (DoubleLinkedListNode*)this->m_Allocator->Malloc( sizeof(DoubleLinkedListNode) )
                    , DoubleLinkedListNode( newItem )
                    , 0 );

    DoubleLinkedListNode *tmp2 = DLListBase::InsertTail( tmp1 );

    return (T*)( tmp2 ? tmp2->m_Item : 0 );
}

template<class T>
T* DLList<T>::DeleteHead( void )
{
    DoubleLinkedListNode *tmp1 = DLListBase::DeleteHead();

    T* tmp2 = (T*)( tmp1 ? tmp1->m_Item : 0 );

    DES_FREE( tmp1, this->m_Allocator->Free, DoubleLinkedListNode );

    return tmp2;
}

template<class T>
T* DLList<T>::DeleteTail( void )
{
    DoubleLinkedListNode *tmp1 = DLListBase::DeleteTail();

    T* tmp2 = (T*)( tmp1 ? tmp1->m_Item : 0 );

    DES_FREE( tmp1, this->m_Allocator->Free, DoubleLinkedListNode );

    return tmp2;
}

template<class T>
int DLList<T>::GetItem( T *&item, int slot )
{
    DoubleLinkedListNode *node = 0;
    int ret = DLListBase::GetNode( node, slot );
    if( 0 == ret )
    {
        item = (T*)(node->m_Item);
    }
    return ret;
}

template<class T>
DoubleLinkedList<T>::DoubleLinkedList( AllocatorBase *alloc )
    :m_Allocator( alloc ), m_Size( 0 )
{
    if( 0 == this->m_Allocator )
    {
        this->m_Allocator = AllocatorBase::GetInstance();
    }
    NEW_MALLOC( this->m_Head, (T*)this->m_Allocator->Malloc(sizeof(T)), T );

    this->InitHead();
}

template<class T>
DoubleLinkedList<T>::DoubleLinkedList( const DoubleLinkedList<T> &dlList )
    :m_Allocator( dlList.m_Allocator )
{
    if( 0 == this->m_Allocator )
    {
        this->m_Allocator = AllocatorBase::GetInstance();
    }
    NEW_MALLOC( this->m_Head, (T*)this->m_Allocator->Malloc(sizeof(T)), T );

    this->InitHead();
    this->CopyNodes( dlList );
    this->m_Size = dlList->m_Size;
}

template<class T>
DoubleLinkedList<T>::~DoubleLinkedList( void )
{
    this->DeleteNodes();

    DES_FREE( this->m_Head, this->m_Allocator->Free, T );

    this->m_Head = 0;
}

template<class T>
void DoubleLinkedList<T>::operator =( const DoubleLinkedList<T> &dlList )
{
    this->DeleteNodes();
    this->CopyNodes( dlList );
}

template<class T>
int DoubleLinkedList<T>::IsEmpty( void ) const
{
    return this->m_Size ? 0 : 1;
}

template<class T>
void DoubleLinkedList<T>::InitHead()
{
    this->m_Head->m_Next = this->m_Head;
    this->m_Head->m_Prev = this->m_Head;
}

template<class T>
int DoubleLinkedList<T>::InsertElement( T *newItem, int before, T *oldItem )
{
    if( 0 == oldItem )
    {
        oldItem = this->m_Head;
    }
    if( before )
    {
        oldItem = static_cast<T*>( oldItem->m_Prev );
    }

    newItem->m_Next = oldItem->m_Next;
    newItem->m_Next->m_Prev = newItem;
    newItem->m_Prev = oldItem;
    oldItem->m_Next = newItem;

    ++this->m_Size;

    return 0;
}

template<class T>
int DoubleLinkedList<T>::RemoveElement( T *oldItem )
{
    if( 0 == oldItem->m_Next || 0 == oldItem->m_Prev || oldItem == this->m_Head || 0 == this->m_Size )
    {
        return -1;
    }

    oldItem->m_Prev->m_Next = oldItem->m_Next;
    oldItem->m_Next->m_Prev = oldItem->m_Prev;

    oldItem->m_Next = oldItem->m_Prev = 0;

    --this->m_Size;

    return 0;
}

template<class T>
T* DoubleLinkedList<T>::InsertHead( T *newItem )
{
    this->InsertElement( newItem );

    return newItem;
}

template<class T>
T* DoubleLinkedList<T>::InsertTail( T *newItem )
{
    this->InsertElement( newItem, 1 );

    return newItem;
}

template<class T>
T* DoubleLinkedList<T>::DeleteHead( void )
{
    if( this->IsEmpty() )
    {
        return 0;
    }
    T *tmp = static_cast<T*>( this->m_Head->m_Next );
    this->RemoveElement( tmp );

    return tmp;
}
template<class T>
T* DoubleLinkedList<T>::DeleteTail( void )
{
    if( this->IsEmpty() )
    {
        return 0;
    }
    T *tmp = static_cast<T*>( this->m_Head->m_Prev );
    this->RemoveElement( tmp );

    return tmp;
}

template<class T>
void DoubleLinkedList<T>::CopyNodes( const DoubleLinkedList<T> &rhs )
{
    for( DoubleLinkedListIterator<T> iter( rhs ); !iter.Done(); iter.DoAdvance() )
    {
        T *tmp = 0;
        NEW_MALLOC( tmp, (T*)this->m_Allocator->Malloc(sizeof(T)), T(*iter->GetCurrent()) );
        this->InsertTail( tmp );
    }
}

template<class T>
void DoubleLinkedList<T>::DeleteNodes( void )
{
    while( !this->IsEmpty() )
    {
        T *tmp = (T*)(this->m_Head->m_Next);
        this->RemoveElement( tmp );
        DES_FREE( tmp, this->m_Allocator->Free, T );
    }
}

template<class T>
int DoubleLinkedList<T>::GetNode( T *&item, int slot )
{
    DoubleLinkedListIterator<T> iter( *this );
    for( int i=0; i<slot && !iter.Done(); ++i )
    {
        iter.DoAdvance();
    }
    item = this->GetCurrent();

    return item ? 0 : -1;
}

template<class T>
DoubleLinkedListReverseIterator<T>::DoubleLinkedListReverseIterator( const DoubleLinkedList<T> &dlList )
    : DoubleLinkedListIteratorBase<T>( dlList )
{
    this->m_Current = static_cast<T*>( dlList.m_Head->m_Prev );
}

template<class T>
DoubleLinkedListReverseIterator<T>& DoubleLinkedListReverseIterator<T>::operator ++( void )
{
    this->DoRetreat();
    return *this;
}
template<class T>
DoubleLinkedListReverseIterator<T>& DoubleLinkedListReverseIterator<T>::operator ++( int )
{
    DoubleLinkedListReverseIterator<T> tmp( *this );
    this->DoRetreat();
    return *this;
}

template<class T>
DoubleLinkedListReverseIterator<T>& DoubleLinkedListReverseIterator<T>::operator --( void )
{
    this->DoAdvance();
    return *this;
}
template<class T>
DoubleLinkedListReverseIterator<T>& DoubleLinkedListReverseIterator<T>::operator --( int )
{
    DoubleLinkedListReverseIterator<T> tmp( *this );
    this->DoAdvance();
    return *this;
}

template<class T>
DoubleLinkedListIterator<T>::DoubleLinkedListIterator( const DoubleLinkedList<T> &dlList )
    :DoubleLinkedListIteratorBase<T>( dlList )
{
    this->m_Current = static_cast<T*>( dlList->m_Head->m_Next );
}

template<class T>
DoubleLinkedListIterator<T>& DoubleLinkedListIterator<T>::operator ++( void )
{
    this->DoAdvance();
    return *this;
}
template<class T>
DoubleLinkedListIterator<T>& DoubleLinkedListIterator<T>::operator ++( int )
{
    DoubleLinkedListIterator<T> tmp( *this );
    this->DoAdvance();
    return tmp;
}

template<class T>
DoubleLinkedListIterator<T>& DoubleLinkedListIterator<T>::operator --( void )
{
    this->DoRetreat();
    return *this;
}
template<class T>
DoubleLinkedListIterator<T>& DoubleLinkedListIterator<T>::operator --( int )
{
    DoubleLinkedListIterator<T> tmp( *this );
    this->DoRetreat();
    return tmp;
}

template<class T>
DoubleLinkedListIteratorBase<T>::DoubleLinkedListIteratorBase( const DoubleLinkedList<T> &dlList )
    :m_Current( 0 ), m_DLList( dlList )
{
}

template<class T>
DoubleLinkedListIteratorBase<T>::DoubleLinkedListIteratorBase( const DoubleLinkedListIteratorBase<T> &iterator )
    :m_Current( iterator.m_Current ), m_DLList( iterator.m_DLList )
{
}

template<class T>
T* DoubleLinkedListIteratorBase<T>::NotDone( void ) const
{
    if( this->m_Current != this->m_DLlist->m_Head )
    {
        return this->m_Current;
    }
    else
    {
        return 0;
    }
}

template<class T>
int DoubleLinkedListIteratorBase<T>::Done( void ) const
{
    return this->NotDone() ? 1 : 0;
}

template<class T>
T* DoubleLinkedListIteratorBase<T>::GetCurrent( void ) const
{
    return this->NotDone();
}

template<class T>
T& DoubleLinkedListIteratorBase<T>::operator *( void ) const
{
    return *( this->NotDone() );
}

template<class T>
T* DoubleLinkedListIteratorBase<T>::DoAdvance( void )
{
    if( this->NotDone() )
    {
        this->m_Current = static_cast<T*>(this->m_Current->m_Next);

        return this->NotDone();
    }
    else
    {
        return 0;
    }
}

template<class T>
T* DoubleLinkedListIteratorBase<T>::DoRetreat( void )
{
    if( this->NotDone() )
    {
        this->m_Current = static_cast<T*>( this->m_Current->m_Prev );
    }
    else
    {
        return 0;
    }
}

#endif
