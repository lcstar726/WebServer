#ifndef __TSS_T_H__
#define __TSS_T_H__

#include "Public.h"
#include "CopyDisabled.h"
#include "OSThread.h"

template<class TYPE>
class Tss: private CopyDisabled
{
public:
    Tss( TYPE *tsObj = NULL );
    virtual ~Tss( void );

    TYPE* operator ->() const;
    operator TYPE*( void ) const;

protected:
    virtual TYPE *MakeTssType( void ) const;
    
    int TssInit( void );
    TYPE * GetTssObj( void ) const;
    
    TYPE* GetTssValue( void ) const;
    int SetTssValue( TYPE *newTssObj );
    
    static void Cleanup( void *ptr );

    ThreadMutex     m_KeyLock;

    volatile bool   m_bOnce;
    thread_key_t    m_ThreadKey;
};

#define TSS_TYPE(T) Tss< T >
#define TSS_GET(I, T) ((I)->operator T*())

template<class TYPE>
Tss<TYPE>::Tss( TYPE *tsObj )
    : m_bOnce( false ), m_ThreadKey( OS::NullThreadKey )
{
    if( tsObj != NULL )
    {
        if( -1 == this->TssInit() )
        {
            return;
        }
        this->SetTssValue( tsObj );
    }
}
template<class TYPE>
Tss<TYPE>::~Tss( void )
{
    if( this->m_bOnce )
    {
        TYPE *tsObj = this->GetTssValue();
        this->SetTssValue( NULL );
        Tss<TYPE>::Cleanup( tsObj );

        //OS::DetachThreadKey (this->m_ThreadKey );
        OS::FreeThreadKey( this->m_ThreadKey );
    }
}

template<class TYPE>
TYPE* Tss<TYPE>::operator ->() const
{
    return this->GetTssObj();
}

template<class TYPE>
Tss<TYPE>::operator TYPE*( void ) const
{
    return this->GetTssObj();
}

template<class TYPE>
TYPE* Tss<TYPE>::MakeTssType( void ) const
{
    TYPE *temp = NULL;
    NEW_RETURN( temp, TYPE, 0 );
    return temp;
}

template<class TYPE>
int Tss<TYPE>::TssInit( void )
{
    GUARD_RETURN( ThreadMutex, ace_mon, this->m_KeyLock, 0 );

    if( !this->m_bOnce )
    {
        if( OS::CreateKey(&this->key_, &Tss<TYPE>::Cleanup) != 0 )
        {
            return -1; // Major problems, this should *never* happen!
        }
        else
        {
            // This *must* come last to avoid race conditions!
            this->m_bOnce = true;
            return 0;
        }
    }

    return 0;
}

template<class TYPE>
TYPE * Tss<TYPE>::GetTssObj( void ) const
{
    if(!this->m_bOnce)
    {
        // Create and initialize thread-specific tsObj.
        if( const_cast< Tss<TYPE> * >(this)->TssInit() == -1 )
        {
            // Seriously wrong..
            return 0;
        }
    }

    TYPE *tsObj = NULL;

    tsObj = this->GetTssValue();

    // Check to see if this is the first time in for this thread.
    if( NULL == tsObj )
    {
        tsObj = this->MakeTssType();

        if( NULL == tsObj )
        {
            return 0;
        }

        // Store the dynamically allocated pointer in thread-specific
        // storage.
        if( this->SetTssValue(tsObj) == -1 )
        {
            delete tsObj;
            return 0; // Major problems, this should *never* happen!
        }
    }

    return tsObj;
}

template<class TYPE>
inline TYPE* Tss<TYPE>::GetTssValue( void ) const
{
    void *temp = NULL;
    if( Thread::GetSpecific(this->m_ThreadKey, &temp) == -1 )
    {
        return NULL; // This should not happen!
    }
    return static_cast<TYPE *>( temp );
}

template<class TYPE>
inline int Tss<TYPE>::SetTssValue( TYPE *newTssObj )
{
    if( Thread::SetSpecific(this->m_ThreadKey, (void *)newTssObj) != 0 )
    {
        return -1;
    }

    return 0;
}

template<class TYPE>
void Tss<TYPE>::Cleanup( void *ptr )
{
    delete (TYPE*) ptr;
}

#endif

