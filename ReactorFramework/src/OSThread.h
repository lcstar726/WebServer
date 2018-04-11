#ifndef __OS_THREAD_H__
#define __OS_THREAD_H__

#include "Public.h"
#include "ThreadAdapter.h"

namespace OS
{
    extern thread_t nullThreadId;
    
    extern hthread_t nullThreadHandle;
    
    extern int ThreadCreate( THR_FUNC func
                            , void *args
                            , long flags
                            , thread_t *thrId
                            , hthread_t *thrHandle
                            , long priority = DEFAULT_THREAD_PRIORITY
                            , void *stack = 0
                            , size_t stackSize = DEFAULT_THREAD_STACKSIZE
                            , ThreadAdapterBase *thrAdapter = 0
                            , const char **thrName = 0 );

    extern thread_key_t NullThreadKey;

    extern int ThreadCreateKeyNative( thread_key_t *key, THR_DEST );
    extern int ThreadCreateKey( thread_key_t *key, THR_DEST );
    //extern int DetachThreadKey( thread_key_t key );

    extern int FreeThreadKeyNative( thread_key_t key );    
    extern int FreeThreadKey( thread_key_t key );

    extern int ThreadGetSpecificNative( thread_key_t key, void **data );
    extern int ThreadGetSpecific( thread_key_t key, void **data );

    extern int ThreadSetSpecificNative( thread_key_t key, void *data );
    extern int ThreadSetSpecific( thread_key_t key, void *data );

}

#endif

