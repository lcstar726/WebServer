#ifndef __Singleton_H__
#define __Singleton_H__

#include "Public.h"
#include "GlobalMacros.h"
#include "ObjectManager.h"
#include "Cleanup.h"
#include "TssT.h"

template<class T, class LOCK>
class Singleton
{
public:
    static T* GetInstance( void );
    
    //virtual void CleanUp( void *param = 0 );
    
    //static void close();
    
protected:
    Singleton( void ){ }
    
    static Singleton<T, LOCK> *&InstanceAux( void );
    
    static Singleton<T, LOCK> *m_pSingleton;
    
    T m_Instance;
};

template<class T, class LOCK>
Singleton<T, LOCK> *Singleton<T, LOCK>::m_pSingleton = 0;
    
template<class T, class LOCK>
Singleton<T, LOCK>*& Singleton<T, LOCK>::InstanceAux( void )
{
    return Singleton<T, LOCK>::m_pSingleton;
}

template<class T, class LOCK>
T* Singleton<T, LOCK>::GetInstance( void )
{
    Singleton<T, LOCK> *&singleton = Singleton<T,LOCK>::InstanceAux();
    if( NULL == singleton )
    {
        if( ObjectManager::StartingUp() || ObjectManager::ShuttingDown() )
        {
            //singleton = new Singleton<T,LOCK>;
            NEW_RETURN( singleton, (Singleton<T, LOCK>), 0 );
        }
    }
    
    return &singleton->m_Instance;
}

template<class TYPE, class LOCK>
class TssSingleton: public Cleanup
{
public:
    static TYPE* GetInstance( void );

    virtual void Cleanup( void *param = NULL );

protected:
    TssSingleton( void );
    
    static TssSingleton<TYPE, LOCK> *m_pSingleton;
    static TssSingleton<TYPE, LOCK> *& GetInstanceAux( void );

    TSS_TYPE( TYPE ) m_Instance;
private:
    TssSingleton( const TssSingleton<TYPE,LOCK> & );
    void operator =( const TssSingleton<TYPE, LOCK> & );
};

template<class TYPE, class LOCK>
inline TssSingleton<TYPE,LOCK>::TssSingleton( void )
{
}

template<class TYPE, class LOCK>
TssSingleton<TYPE, LOCK> *& TssSingleton<TYPE,LOCK>::GetInstanceAux( void )
{
    return TssSingleton<TYPE, LOCK>::m_pSingleton;
}

template<class TYPE, class LOCK>
TYPE* TssSingleton<TYPE,LOCK>::GetInstance( void )
{
    TssSingleton<TYPE, LOCK> *&singleton = TssSingleton<TYPE, LOCK>::GetInstanceAux();

    if( NULL == singleton )
    {
        if( ObjectManager::StartingUp() || ObjectManager::ShuttingDown() )
        {
            //singleton = new Singleton<T,LOCK>;
            NEW_RETURN( singleton, (TssSingleton<TYPE, LOCK>), 0 );
        }
    }
    return TSS_GET( &singleton->m_Instance, TYPE );
}

template<class TYPE, class LOCK>
void TssSingleton<TYPE,LOCK>::Cleanup( void * )
{
    delete this;
    TssSingleton<TYPE,LOCK>::GetInstanceAux() = NULL;
}

template<class TYPE, class LOCK>
TssSingleton<TYPE, LOCK> *TssSingleton<TYPE, LOCK>::m_pSingleton = NULL;

#endif
