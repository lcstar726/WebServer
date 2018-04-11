#include "OSThread.h"
#include <pthread.h>
#include "Errno.h"
#include "UniquePtr.h"

thread_t OS::nullThreadId;

hthread_t OS::nullThreadHandle;

thread_key_t OS::NullThreadKey = static_cast<thread_key_t>( -1 );

int OS::ThreadCreate( THR_FUNC func
                            , void *args
                            , long flags
                            , thread_t *thrId
                            , hthread_t *thrHandle
                            , long priority
                            , void *stack
                            , size_t stackSize
                            , ThreadAdapterBase *thrAdapter
                            , const char **thrName )
{
    if( BIT_DISABLED(flags, THR_DETACHED) && BIT_DISABLED(flags, THR_JOINABLE) )
    {
        SET_BITS( flags, THR_JOINABLE );
    }

    ThreadAdapterBase *thrArgs = 0;
    if( 0 == thrAdapter )
    {
        NEW_RETURN( thrArgs, OSThreadAdapter(func, thrArgs, (THR_C_FUNC)THREAD_ADAPTER_NAME, flags), -1 );
    }
    else
    {
        thrArgs = thrAdapter;
    }
    UniquePtr<ThreadAdapterBase> uniqueArgs;
    if( 0 == thrAdapter )
    {
        uniqueArgs.ResetNativePtr( thrArgs );
    }

    thread_t tmpThr;
    if( 0 == thrId )
    {
        thrId = &tmpThr;
    }
    hthread_t tmpThrHandle;
    if( 0 == thrHandle )
    {
        thrHandle = &tmpThrHandle;
    }

    int result;
    pthread_attr_t attr;

    if( (result = ADAPT_RETVAL( pthread_attr_init(&attr), result )) != 0 )
    {
        return -1;
    }

    if( stackSize != 0 )
    {
        size_t size = stackSize;
        if( stack != 0 )
        {
            result = ADAPT_RETVAL( pthread_attr_setstack(&attr, stack, size), result );
        }
        else
        {
            result = ADAPT_RETVAL( pthread_attr_setstacksize(&attr, size), result );
        }
        if( -1 == result )
        {
            pthread_attr_destroy( &attr );
            return -1;
        }
    }

    if( flags != 0 )
    {
        if( BIT_ENABLED( flags, THR_DETACHED) || BIT_ENABLED( flags, THR_JOINABLE ) )
        {
            int dstat = PTHREAD_CREATE_JOINABLE;
            if( BIT_ENABLED(flags, THR_DETACHED) )
            {
                dstat = PTHREAD_CREATE_DETACHED;
            }
            if( (result = ADAPT_RETVAL( pthread_attr_setdetachstate(&attr, dstat), result )) != 0 )
            {
                pthread_attr_destroy( &attr );
                return -1;
            }
        }
        
        if( priority != DEFAULT_THREAD_PRIORITY )
        {
            SET_BITS( flags, THR_EXPLICIT_SCHED );
            if( BIT_DISABLED(flags, THR_SCHED_FIFO) && BIT_DISABLED(flags, THR_SCHED_RR) && BIT_DISABLED(flags, THR_SCHED_DEFAULT) )
            {
                SET_BITS( flags, THR_SCHED_DEFAULT );
            }
        }

        if( BIT_ENABLED(flags, THR_SCHED_FIFO) || BIT_ENABLED(flags, THR_SCHED_RR) || BIT_ENABLED(flags, THR_SCHED_DEFAULT) )
        {
            int spolicy;
        
            SET_BITS( flags, THR_EXPLICIT_SCHED );
            if( BIT_ENABLED(flags, THR_SCHED_DEFAULT ) )
            {
                spolicy = SCHED_OTHER;
            }
            else if( BIT_ENABLED(flags, THR_SCHED_FIFO) )
            {
                spolicy = SCHED_FIFO;
            }
            else
            {
                spolicy = SCHED_RR;
            }
            ADAPT_RETVAL( pthread_attr_setschedpolicy(&attr, spolicy), result );
            if( result != 0 )
            {
                pthread_attr_destroy( &attr );
                return -1;
            }
        }

        if( (BIT_ENABLED(flags, THR_SCHED_FIFO) || BIT_ENABLED(flags, THR_SCHED_RR) || BIT_ENABLED(flags, THR_SCHED_DEFAULT))
            && priority == DEFAULT_THREAD_PRIORITY )
        {
            if( BIT_ENABLED(flags, THR_SCHED_FIFO) )
            {
                priority = THR_PRI_FIFO_DEF;
            }
            else if( BIT_ENABLED(flags, THR_SCHED_RR) )
            {
                priority = THR_PRI_RR_DEF;
            }
            else
            {
                priority = THR_PRI_OTHER_DEF;
            }
        }

        if( priority != DEFAULT_THREAD_PRIORITY )
        {
            struct sched_param sparam;
            memset( &sparam, 0x00, sizeof(sparam) );

            if( BIT_ENABLED(flags, THR_SCHED_FIFO) )
            {
                sparam.sched_priority = MIN( THR_PRI_FIFO_MAX, MAX(THR_PRI_FIFO_MIN, priority) );
            }
            else if( BIT_ENABLED(flags, THR_SCHED_RR) )
            {
                sparam.sched_priority = MIN( THR_PRI_RR_MAX, MAX(THR_PRI_RR_MIN, priority) );
            }
            else
            {
                sparam.sched_priority = MIN( THR_PRI_OTHER_MAX, MAX(THR_PRI_OTHER_MIN, priority) );
            }
            ADAPT_RETVAL( pthread_attr_setschedparam(&attr, &sparam), result );
            if( result != 0 )
            {
                pthread_attr_destroy( &attr );
                return -1;
            }
        }

        if( BIT_ENABLED(flags, THR_INHERIT_SCHED) || BIT_ENABLED(flags, THR_EXPLICIT_SCHED) )
        {
            int sched = PTHREAD_EXPLICIT_SCHED;
            if( BIT_ENABLED(flags, THR_INHERIT_SCHED) )
            {
                sched = PTHREAD_INHERIT_SCHED;
            }
            if( ADAPT_RETVAL( pthread_attr_setinheritsched(&attr, sched), result ) != 0 )
            {
                pthread_attr_destroy( &attr );
                return -1;
            }
        }

        if( BIT_ENABLED(flags, THR_SCOPE_SYSTEM) || BIT_ENABLED(flags, THR_SCOPE_PROCESS) )
        {
            int scope = PTHREAD_SCOPE_SYSTEM;
            if( BIT_ENABLED(flags, THR_SCOPE_SYSTEM) )
            {
                scope = PTHREAD_SCOPE_SYSTEM;
            }
            if( ADAPT_RETVAL( pthread_attr_setscope(&attr, scope), result ) != 0 )
            {
                pthread_attr_destroy( &attr );
                return -1;
            }
        }

        if( BIT_ENABLED(flags, THR_NEW_LWP) )
        {
            int lwps = pthread_getconcurrency();
            if( -1 == lwps )
            {
                if( errno == ENOTSUP )
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            else if( pthread_setconcurrency(lwps+1) == -1 )
            {
                if( ENOTSUP == errno )
                {
                    //nothing to do :)
                }
                else
                {
                    return -1;
                }
            }
        }
    }//if( flags != 0 )
    OSCALL( ADAPT_RETVAL(pthread_create( thrId, &attr, thrArgs->GetEntryPoint(), thrArgs ), result), int, -1, result );

    pthread_attr_destroy( &attr );

    if( result != -1 )
    {
        *thrHandle = *thrId;
    }

    uniqueArgs.Release();

    return result;
}

int OS::ThreadCreateKeyNative( thread_key_t *key, THR_DEST )
{
    int result;
    OSCALL_RETURN( ADAPT_RETVAL( pthread_key_create(key, dest), result ), int, -1 );
}

int OS::ThreadCreateKey( thread_key_t *key, THR_DEST )
{
    return  OS::ThreadCreateKeyNative(key, dest);
}

int OS::FreeThreadKeyNative( thread_key_t key )
{
    return pthread_key_delete( key );
}
int OS::FreeThreadKey( thread_key_t key )
{
    return OS::FreeThreadKeyNative(key);
}

int OS::ThreadGetSpecificNative( thread_key_t key, void **data )
{
    *data = pthread_getspecific( key );
    return 0;
}
int OS::ThreadGetSpecific( thread_key_t key, void **data )
{
    return OS::ThreadGetSpecificNative (key, data);
}

int OS::ThreadSetSpecificNative( thread_key_t key, void *data )
{
    int result;
    OSCALL_RETURN( ADAPT_RETVAL(pthread_setspecific(key, data), result), int, -1 );
}
int OS::ThreadSetSpecific( thread_key_t key, void *data )
{
    return OS::ThreadSetSpecificNative( key, data );
}


