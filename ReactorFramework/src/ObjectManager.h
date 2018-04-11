#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "Public.h"
#include "GlobalMacros.h"
#include "Errno.h"
#include "NullMutex.h"
#include "ThreadMutex.h"

class ObjectManagerBase
{
public:
    virtual int Init( void ) = 0;
    
    virtual int Fini( void ) = 0;
    
    enum enumObjectManagerState
    {
        OBJ_MAN_UNINITIALIZED = 0,
        OBJ_MAN_INITIALIZING,
        OBJ_MAN_INITIALIZED,
        OBJ_MAN_SHUTTING_DOWN,
        OBJ_MAN_SHUT_DOWN
    };    

protected:
    ObjectManagerBase( void );
    virtual ~ObjectManagerBase( void );
    
    bool StartingUpAux( void );
    
    bool ShuttingDownAux( void );
    
    enumObjectManagerState m_ObjectMgrState;

private:
    ObjectManagerBase( const ObjectManagerBase & );
    ObjectManagerBase& operator=( const ObjectManagerBase & );
};

class ObjectManager: public ObjectManagerBase
{
public:
    ObjectManager( void );
    ~ObjectManager( void );

    virtual int Init( void );
    virtual int Fini( void );
    
    enum enumPreAllocatedObject
    {
        FILECACHE_LOCK = 0
        , STATIC_OBJECT_LOCK
        , MT_CORBA_HANDLER_LOCK
        , DUMP_LOCK
        , SIG_HANDLER_LOCK
        , SINGLETON_NULL_LOCK
        , SINGLETON_RECURSIVE_THREAD_LOCK
        , THREAD_EXIT_LOCK
        , TOKEN_MANAGER_CREATION_LOCK
        , TOKEN_INVARIANTS_CREATION_LOCK
        , PROACTOR_EVENT_LOOP_LOCK
        , APPLICATION_PREALLOCATED_OBJECT_DECLARATIONS
        
        , PREALLOCATED_OBJECTS
    };    
    
    static ObjectManager* GetInstance();
    
    static void* m_PreAllocatedObjectObject[PREALLOCATED_OBJECTS];
    //static void* m_PreAllocatedObjectArray[PREALLOCATED_OBJECTS];
    
    static bool StartingUp( void );
    static bool ShuttingDown( void );

private:
    static ObjectManager *m_Instance;
    
    RecursiveThreadMutex *m_InternalLock;

    CleanupAdapter<NullMutex> *m_SingletonNullMutex;
    CleanupAdapter<RecursiveThreadMutex> *m_SingletonRecursiveMutex;
    
    ObjectManager( const ObjectManager & );
    ObjectManager& operator= ( const ObjectManager & );
};


#define PREALLOCATE_OBJECT(TYPE, ID)\
    {\
      CleanupAdapter<TYPE> *obj_p;\
      NEW_RETURN (obj_p, CleanupAdapter<TYPE>, -1);\
      m_PreAllocatedObjectObject[ID] = obj_p;\
    }

template<class T>
class ManagedObject
{
public:
    static T* GetPreAllocatedObject( ObjectManager::enumPreAllocatedObject identifier )
    {
        return &( (CleanupAdapter<T>*)ObjectManager::m_PreAllocatedObjectObject[identifier] )->GetObject();
    }

protected:
    ManagedObject( void ) { }

private:
    ManagedObject( const ManagedObject& );
    void operator= ( const ManagedObject& );
    
};

class StaticObjectLock
{
public:
    static RecursiveThreadMutex* GetInstance( void );
    
    static void CleanupLock( void );
};

typedef CleanupAdapter<RecursiveThreadMutex> StaticObjectLockType;
static StaticObjectLockType *g_StaticObjLock = 0;

#endif

