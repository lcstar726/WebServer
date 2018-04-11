#ifndef __CLEAN_UP_H__
#define __CLEAN_UP_H__

#include "Public.h"

class Cleanup
{
public:
    Cleanup( void ) { }
    
    virtual ~Cleanup( void ) { }
    
    virtual void CleanUp( void *param );
};

template<class T>
class CleanupAdapter: public Cleanup
{
public:
    CleanupAdapter( void ) { }
    
    ~CleanupAdapter( void ) { }
    
    T &GetObject( void );

private:
    CleanupAdapter( const CleanupAdapter<T>& );
    void operator=( const CleanupAdapter<T>& );
    
    T m_Object;
};

template<class T>
inline T& CleanupAdapter<T>::GetObject( void )
{
    return this->m_Object;
}
#endif

