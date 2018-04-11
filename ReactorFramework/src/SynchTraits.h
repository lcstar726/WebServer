#ifndef __SYNCH_TRAITS_H__
#define __SYNCH_TRAITS_H__

#include "NullMutex.h"
#include "ThreadMutex.h"
#include "SynchGuard.h"

class MTSynch
{
public:
    typedef ThreadMutex             MUTEX;
    typedef NullMutex               NULL_MUTEX;
    typedef RecursiveThreadMutex    RECURSIVE_MUTEX;
};

#define SYNCH_MUTEX             MTSynch::MUTEX
#define SYNCH_NULL_MUTEX        MTSynch::NULL_MUTEX
#define SYNCH_RECURSIVE_MUTEX   MTSynch::RECURSIVE_MUTEX

#define SYNCH_DECL      typename _SYNCH
#define SYNCH_USE       _SYNCH
#define SYNCH_MUTEX_T   typename _SYNCH::MUTEX

#if !defined (DEFAULT_THREAD_MANAGER_LOCK)
#define DEFAULT_THREAD_MANAGER_LOCK     SYNCH_MUTEX
#endif

#if !defined (READ_GUARD_RETURN)
#define READ_GUARD_RETURN(MUTEX,OBJ,LOCK,RETURN) \
  ReadGuard< MUTEX > OBJ (LOCK); \
    if (OBJ.IsLocked () == 0) return RETURN;
#endif

#if !defined (GUARD_ACTION)
#define GUARD_ACTION(MUTEX, OBJ, LOCK, ACTION, REACTION) \
   SynchGuard< MUTEX > OBJ (LOCK); \
   if (OBJ.IsLocked() != 0) { ACTION; } \
   else { REACTION; }
#endif

#if !defined (GUARD_REACTION)
#define GUARD_REACTION(MUTEX, OBJ, LOCK, REACTION) \
  GUARD_ACTION(MUTEX, OBJ, LOCK, ;, REACTION)
#endif

#if !defined (GUARD_RETURN)
#define GUARD_RETURN(MUTEX, OBJ, LOCK, RETURN) \
  GUARD_REACTION(MUTEX, OBJ, LOCK, return RETURN)
#endif

#if !defined (SYNCH_GUARD)
#define SYNCH_GUARD(MUTEX, OBJ, LOCK) \
  GUARD_REACTION(MUTEX, OBJ, LOCK, return)
#endif

#endif
