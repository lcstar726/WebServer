#include "Allocator.h"
#include "SynchTraits.h"
#include "ThreadMutex.h"

Allocator *Allocator::m_Instance = NULL;
int Allocator::m_DeleteAllocator = NULL;

void* NewAllocator::Malloc( size_t nBytes )
{
    char *ptr = NULL;
    if( nBytes > 0 )
    {
        NEW_RETURN( ptr, char[nBytes], 0 );
    }

    return (void*)ptr;
}

void NewAllocator::Free( void *ptr )
{
    delete[] (char*)ptr;
}

Allocator* Allocator::GetInstance( void )
{
    if( NULL == Allocator::m_Instance )
    {
        GUARD_RETURN( RecursiveThreadMutex, monitor, *StaticObjectLock::GetInstance(), 0 );
        static void *instance = NULL;

        Allocator::m_Instance = new( &instance )NewAllocator;
    }

    return Allocator::m_Instance;
}

Allocator* Allocator::SetInstance( Allocator *newAlloc )
{
    GUARD_RETURN( RecursiveThreadMutex, monitor, *StaticObjectLock::GetInstance(), 0 );

    Allocator *oldAlloc = Allocator::m_Instance;

    Allocator::m_Instance = newAlloc;

    Allocator::m_DeleteAllocator = NULL;

    return oldAlloc;
}

