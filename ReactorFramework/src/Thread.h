#ifndef __THREAD_H__
#define __THREAD_H__

#include "Public.h"

class Thread
{
public:
    static thread_t NullThread;
    static thread_t GetThreadSelf( void );

    static int ThreadEqual( thread_t t1, thread_t t2 );

    static int CreateKey( thread_key_t *keyp, THR_DEST destructor );

    static int GetSpecific( thread_key_t key, void **valuep );
    static int SetSpecific( thread_key_t key, void *value );

private:
    Thread( void );
};

#endif

