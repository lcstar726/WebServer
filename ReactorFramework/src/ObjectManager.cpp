#include "ObjectManager.h"

ObjectManagerBase::ObjectManagerBase( void ): m_ObjectMgrState( OBJ_MAN_UNINITIALIZED )
{
}
ObjectManagerBase::~ObjectManagerBase( void )
{
}

bool ObjectManagerBase::StartingUpAux( void )
{
    return this->m_ObjectMgrState < OBJ_MAN_INITIALIZED;
}

bool ObjectManagerBase::ShuttingDownAux( void )
{
    return this->m_ObjectMgrState > OBJ_MAN_INITIALIZED;
}

ObjectManager* ObjectManager::m_Instance = 0;
    
void* ObjectManager::m_PreAllocatedObjectObject[PREALLOCATED_OBJECTS] = { 0 };

ObjectManager::ObjectManager( void )
    :m_SingletonNullMutex( 0 ), m_SingletonRecursiveMutex( 0 )
{
    NEW( this->m_InternalLock, RecursiveThreadMutex );
    
    if( 0 == m_Instance )
    {
        m_Instance = this;
    }
    Init();
}

ObjectManager::~ObjectManager( void )
{
    delete this->m_InternalLock;
    
    this->m_InternalLock = 0;
}

int ObjectManager::Init( void )
{
    if( StartingUpAux() )
    {
        m_ObjectMgrState = OBJ_MAN_INITIALIZING;

        if( this == m_Instance )
        {
            PREALLOCATE_OBJECT( RecursiveThreadMutex, STATIC_OBJECT_LOCK )
            PREALLOCATE_OBJECT( ThreadMutex,MT_CORBA_HANDLER_LOCK )
            PREALLOCATE_OBJECT( ThreadMutex, DUMP_LOCK )
            PREALLOCATE_OBJECT( RecursiveThreadMutex, SIG_HANDLER_LOCK )
            PREALLOCATE_OBJECT( NullMutex, SINGLETON_NULL_LOCK )
            PREALLOCATE_OBJECT( RecursiveThreadMutex, SINGLETON_RECURSIVE_THREAD_LOCK )
            PREALLOCATE_OBJECT( ThreadMutex, THREAD_EXIT_LOCK )
        }
        m_ObjectMgrState = OBJ_MAN_INITIALIZED;

        return 0;
    }
    return 1;
}
int ObjectManager::Fini( void )
{
    return 0;
}

ObjectManager* ObjectManager::GetInstance( void )
{
    if( 0 == m_Instance )
    {
        ObjectManager *objMgr = 0;
        
        NEW_RETURN( objMgr, ObjectManager, 0 );
        
        if( objMgr == m_Instance )
        {
            return objMgr;
        }
        cout<<"ObjectManager::GetInstance() failed!"<<endl;
    }
    
    return m_Instance;
}

bool ObjectManager::StartingUp( void )
{
    return ObjectManager::m_Instance ? m_Instance->StartingUpAux() : true;
}
bool ObjectManager::ShuttingDown( void )
{
    return ObjectManager::m_Instance ? m_Instance->ShuttingDownAux() : true;
}

RecursiveThreadMutex* StaticObjectLock::GetInstance( void )
{
    if( ObjectManager::StartingUp() || ObjectManager::ShuttingDown() )
    {
        cout<<"StaticObjectLock::GetInstance call NEW_RETURN( g_StaticObjLock, CleanupAdapter<RecursiveThreadMutex>, 0 )"<<endl;
        NEW_RETURN( g_StaticObjLock, CleanupAdapter<RecursiveThreadMutex>, 0 );
        
        return &g_StaticObjLock->GetObject();
    }
    else
    {
        cout<<"StaticObjectLock::GetInstance call ManagedObject<RecursiveThreadMutex>::GetPreAllocatedObject()"<<endl;
        return ManagedObject<RecursiveThreadMutex>::GetPreAllocatedObject( ObjectManager::STATIC_OBJECT_LOCK );
    }
}

void StaticObjectLock::CleanupLock( void )
{
    delete g_StaticObjLock;
}

