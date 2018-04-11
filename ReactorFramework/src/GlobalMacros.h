#ifndef __GLOBAL_MACROS_H__
#define __GLOBAL_MACROS_H__

#define POW(X) (((X) == 0)?1:(X-=1,X|=X>>1,X|=X>>2,X|=X>>4,X|=X>>8,X|=X>>16,(++X)))
#define EVEN(NUM) (((NUM) & 1) == 0)
#define ODD(NUM) (((NUM) & 1) == 1)
#define BIT_ENABLED(WORD, BIT) (((WORD) & (BIT)) != 0)
#define BIT_DISABLED(WORD, BIT) (((WORD) & (BIT)) == 0)
#define BIT_CMP_MASK(WORD, BIT, MASK) (((WORD) & (BIT)) == MASK)
#define SET_BITS(WORD, BITS) (WORD |= (BITS))
#define CLR_BITS(WORD, BITS) (WORD &= ~(BITS))

#if !defined (DEFAULT_THREAD_MANAGER_PREALLOC)
#define DEFAULT_THREAD_MANAGER_PREALLOC     0
#endif

#if !defined (DEFAULT_THREAD_MANAGER_LWM)
#define DEFAULT_THREAD_MANAGER_LWM      1
#endif

#if !defined (DEFAULT_THREAD_MANAGER_INC)
#define DEFAULT_THREAD_MANAGER_INC      1
#endif

#if !defined (DEFAULT_THREAD_MANAGER_HWM)
#define DEFAULT_THREAD_MANAGER_HWM      20480
#endif

#if !defined (DEFAULT_THREAD_PRIORITY)
#define DEFAULT_THREAD_PRIORITY (-0x7fffffffL - 1L)
#endif

#if !defined (DEFAULT_THREAD_STACKSIZE)
#define DEFAULT_THREAD_STACKSIZE 0
#endif

#undef THR_BOUND
#define THR_BOUND               0x00000001

#undef THR_NEW_LWP
#define THR_NEW_LWP             0x00000002

#undef THR_DAEMON
#define THR_DAEMON              0x00000010

#undef THR_DETACHED
#define THR_DETACHED            0x00000040

#undef THR_SUSPENDED
#define THR_SUSPENDED           0x00000080

#define THR_BOUND               0x00000001
#define THR_NEW_LWP             0x00000002
#define THR_DAEMON              0x00000010
#define THR_DETACHED            0x00000040
#define THR_SUSPENDED           0x00000080
#define THR_SCHED_FIFO          0x00020000
#define THR_SCHED_RR            0x00040000
#define THR_SCHED_DEFAULT       0x00080000

#define THR_SCHED_FIFO          0x00020000
#define THR_SCHED_RR            0x00040000
#define THR_SCHED_DEFAULT       0x00080000
#define THR_JOINABLE            0x00010000
#define THR_SCOPE_SYSTEM        0x00100000
#define THR_SCOPE_PROCESS       0x00200000
#define THR_INHERIT_SCHED       0x00400000
#define THR_EXPLICIT_SCHED      0x00800000
#define THR_SCHED_IO            0x01000000

#if !defined (INVALID_HANDLE)
#define INVALID_HANDLE -1
#endif

#if !defined (PTHREAD_CREATE_JOINABLE)
#define PTHREAD_CREATE_JOINABLE 0
#endif

#if !defined (PTHREAD_CREATE_DETACHED)
#define PTHREAD_CREATE_DETACHED 1
#endif


#if !defined (USYNC_THREAD)
#define USYNC_THREAD PTHREAD_PROCESS_PRIVATE
#endif

#if !defined (USYNC_PROCESS)
#define USYNC_PROCESS PTHREAD_PROCESS_SHARED
#endif

#if !defined (HAS_RECURSIVE_MUTEXES)
#define HAS_RECURSIVE_MUTEXES
#endif

#define PROC_PRI_FIFO_MIN  0
#define PROC_PRI_RR_MIN    0
#define PROC_PRI_OTHER_MIN 0
#define PROC_PRI_FIFO_MAX  59
#define PROC_PRI_RR_MAX    59
#define PROC_PRI_OTHER_MAX 59

#if !defined(PROC_PRI_FIFO_DEF)
#define PROC_PRI_FIFO_DEF (PROC_PRI_FIFO_MIN + (PROC_PRI_FIFO_MAX - PROC_PRI_FIFO_MIN)/2)
#endif
#if !defined(PROC_PRI_RR_DEF)
#define PROC_PRI_RR_DEF (PROC_PRI_RR_MIN + (PROC_PRI_RR_MAX - PROC_PRI_RR_MIN)/2)
#endif
#if !defined(PROC_PRI_OTHER_DEF)
#define PROC_PRI_OTHER_DEF (PROC_PRI_OTHER_MIN + (PROC_PRI_OTHER_MAX - PROC_PRI_OTHER_MIN)/2)
#endif
#if !defined (THR_PRI_FIFO_MIN)
#define THR_PRI_FIFO_MIN  (long) PROC_PRI_FIFO_MIN
#endif
#if !defined (THR_PRI_FIFO_MAX)
#define THR_PRI_FIFO_MAX  (long) PROC_PRI_FIFO_MAX
#endif
#if !defined (THR_PRI_RR_MIN)
#define THR_PRI_RR_MIN    (long) PROC_PRI_RR_MIN
#endif
#if !defined (THR_PRI_RR_MAX)
#define THR_PRI_RR_MAX    (long) PROC_PRI_RR_MAX
#endif
#if !defined (THR_PRI_OTHER_MIN)
#define THR_PRI_OTHER_MIN (long) PROC_PRI_OTHER_MIN
#endif
#if !defined (THR_PRI_OTHER_MAX)
#define THR_PRI_OTHER_MAX (long) PROC_PRI_OTHER_MAX
#endif
#if !defined(THR_PRI_FIFO_DEF)
#define THR_PRI_FIFO_DEF  ((THR_PRI_FIFO_MIN + THR_PRI_FIFO_MAX)/2)
#endif
#if !defined(THR_PRI_RR_DEF)
#define THR_PRI_RR_DEF ((THR_PRI_RR_MIN + THR_PRI_RR_MAX)/2)
#endif
#if !defined(THR_PRI_OTHER_DEF)
#define THR_PRI_OTHER_DEF ((THR_PRI_OTHER_MIN + THR_PRI_OTHER_MAX)/2)
#endif


typedef pthread_t           thread_t;
typedef pthread_t           hthread_t;

typedef pthread_mutex_t     mutex_t;
typedef pthread_mutex_t     thread_mutex_t;
typedef pthread_mutex_t     recursive_thread_mutext_t;
typedef pthread_cond_t      cond_t;
typedef pthread_condattr_t  condattr_t;
typedef pthread_mutexattr_t mutexattr_t;

typedef pthread_key_t       thread_key_t;

typedef void* THR_FUNC_RETURN;
typedef THR_FUNC_RETURN (*THR_FUNC)(void *);
extern "C"
{
    typedef THR_FUNC_RETURN (*THR_C_FUNC)(void * );
}

extern "C"
{
    typedef void ( *THR_C_DEST )( void * );
}
typedef void ( *THR_DEST )( void * );

#define ADAPT_RETVAL(OP,RESULT) ((RESULT = (OP)) != 0 ? (errno = RESULT, -1) : 0)

#define NEW_RETURN(POINTER,CONSTRUCTOR,RET_VAL) \
   do { POINTER = new (nothrow) CONSTRUCTOR; \
     if (POINTER == 0) { errno = ENOMEM; return RET_VAL; } \
   } while (0)
#define NEW(POINTER,CONSTRUCTOR) \
   do { POINTER = new(nothrow) CONSTRUCTOR; \
     if (POINTER == 0) { errno = ENOMEM; return; } \
   } while (0)
#define NEW_NORETURN(POINTER,CONSTRUCTOR) \
   do { POINTER = new(nothrow) CONSTRUCTOR; \
     if (POINTER == 0) { errno = ENOMEM; } \
   } while (0)

#define OSCALL_RETURN(X,TYPE,FAILVALUE) \
  do \
    return (TYPE) (X); \
  while (0)
#define OSCALL(X,TYPE,FAILVALUE,RESULT) \
  do \
    RESULT = (TYPE) (X); \
  while (0)

#define ALLOCATOR_RETURN(POINTER,ALLOCATOR,RET_VAL) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM; return RET_VAL; } \
     } while (0)
#define ALLOCATOR(POINTER,ALLOCATOR) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM; return; } \
     } while (0)
#define ALLOCATOR_NORETURN(POINTER,ALLOCATOR) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM; } \
     } while (0)
  
#define NEW_MALLOC_RETURN(POINTER,ALLOCATOR,CONSTRUCTOR,RET_VAL) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM; return RET_VAL;} \
       else { (void) new (POINTER) CONSTRUCTOR; } \
     } while (0)
#define NEW_MALLOC(POINTER,ALLOCATOR,CONSTRUCTOR) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM; return;} \
       else { (void) new (POINTER) CONSTRUCTOR; } \
     } while (0)
#define NEW_MALLOC_NORETURN(POINTER,ALLOCATOR,CONSTRUCTOR) \
     do { POINTER = ALLOCATOR; \
       if (POINTER == 0) { errno = ENOMEM;} \
       else { (void) new (POINTER) CONSTRUCTOR; } \
     } while (0)

#define DES_FREE(POINTER,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            (POINTER)->~CLASS (); \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)

#define DES_ARRAY_FREE(POINTER,SIZE,DEALLOCATOR,CLASS) \
   do { \
        if (POINTER) \
          { \
            for (size_t i = 0; \
                 i < SIZE; \
                 ++i) \
            { \
              (&(POINTER)[i])->~CLASS (); \
            } \
            DEALLOCATOR (POINTER); \
          } \
      } \
   while (0)

template <class T>
inline const T &
g_min (const T &t1, const T &t2)
{
  return t2 > t1 ? t1 : t2;
}

template <class T>
inline const T &
g_max (const T &t1, const T &t2)
{
  return t1 > t2 ? t1 : t2;
}

template <class T>
inline const T &
g_min (const T &t1, const T &t2, const T &t3)
{
  return g_min (g_min (t1, t2), t3);
}

template <class T>
inline const T &
g_max (const T &t1, const T &t2, const T &t3)
{
  return g_max (g_max (t1, t2), t3);
}

template <class T>
inline const T &
g_range (const T &min, const T &max, const T &val)
{
  return g_min (g_max (min, val), max);
}

#define MIN(a,b)     g_min((a),(b))
#define MAX(a,b)     g_max((a),(b))
#define RANGE(a,b,c) g_range((a),(b),(c))

template<bool>
struct CompileTimeChecker
{
    CompileTimeChecker(...);
};
template<> struct CompileTimeChecker<false> {};
#define STATIC_CHECK(expr,msg) \
{ \
    class Error_##msg {}; \
    (void)sizeof CompileTimeChecker<expr>(Error_##msg()); \
}

template<class To, class From>
To SafeReinterpretCast( From from )
{
    STATIC_CHECK(sizeof(From)<=sizeof(To), DestinationTypeTooNarrow);
    return static_cast<To>( from );
};

template<bool BOOL, class TA, class TB>
struct IfThenElse;

template<class TA, class TB>
struct IfThenElse<true, TA, TB>
{
    typedef TA Result;
};
template<class TA, class TB>
struct IfThenElse<false, TA, TB>
{
    typedef TB Result;
};

#endif

