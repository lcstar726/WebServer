#ifndef __ALLOCATER_H__
#define __ALLOCATER_H__

#include "Public.h"

class Allocator
{
public:
    typedef size_t size_type;

    Allocator( void ) { }

    virtual ~Allocator( void ) { }

    static Allocator* GetInstance( void );

    static Allocator* SetInstance( Allocator * );

    virtual void* Malloc( size_t nBytes ) = 0;
    virtual void Free( void *ptr ) = 0;

private:
    static Allocator *m_Instance;
    static int m_DeleteAllocator;
};

class NewAllocator: public Allocator
{
public:
    virtual void* Malloc( size_t nBytes );
    virtual void Free( void *ptr );
};

#endif

