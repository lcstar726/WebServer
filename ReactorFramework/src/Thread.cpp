#include "Thread.h"

thread_t Thread::NullThread;

thread_t Thread::GetThreadSelf( void )
{
    return pthread_self();
}

int Thread::ThreadEqual( thread_t t1, thread_t t2 )
{
    return pthread_equal( t1, t2 );
}

int Thread::CreateKey( thread_key_t *keyp, THR_DEST destructor )
{
    return OS::ThreadCreateKey( keyp, destructor );
}

int Thread::GetSpecific( thread_key_t key, void **valuep )
{
    return OS::ThreadGetSpecific( key, valuep );
}

int Thread::SetSpecific( thread_key_t key, void *value )
{
    return OS::ThreadSetSpecific( key, value);
}


