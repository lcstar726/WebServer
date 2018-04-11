#ifndef __THREAD_ADAPTER_H__
#define __THREAD_ADAPTER_H__

#include "base.h"
#include "GlobalMacros.h"
#include "SynchTraits.h"
#include "Containers.h"

#define THREAD_ADAPTER_NAME g_FuncThreadAdapter

extern "C" THR_FUNC_RETURN THREAD_ADAPTER_NAME( void *args );

class ThreadManager;
class TaskBase;
class ThreadDescriptor;

class OSThreadDescriptor
{
public:
    virtual ~OSThreadDescriptor( void ) { }
    
    long GetThreadFlags( void ) const { return this->m_ThreadFlags; }

protected:
    OSThreadDescriptor( long flags = 0 ): m_ThreadFlags( flags ) { }
    
    long m_ThreadFlags;
};

class ThreadDescriptorBase: public OSThreadDescriptor
{
    friend class ThreadManager;
    friend class DoubleLinkedList<ThreadDescriptorBase>;
    friend class DoubleLinkedListIterator<ThreadDescriptorBase>;
    friend class DoubleLinkedListIteratorBase<ThreadDescriptorBase>;
    friend class DoubleLinkedList<ThreadDescriptor>;
    friend class DoubleLinkedListIterator<ThreadDescriptor>;
    friend class DoubleLinkedListIteratorBase<ThreadDescriptor>;
    
public:
    ThreadDescriptorBase( void );
    
    virtual ~ThreadDescriptorBase( void ) { }
    
    int GetGroupId( void ) const;
    
    unsigned int GetCurrentState( void ) const;
    
    TaskBase* GetTask( void ) const;

protected:
    void Reset( void );
    
    thread_t m_ThreadId;
    
    hthread_t m_ThreadHandle;
    
    int m_GroupId;
    
    unsigned int m_CurrentState;
    
    TaskBase *m_Task;
    
    ThreadDescriptorBase *m_Next;
    ThreadDescriptorBase *m_Prev;
};

class ThreadDescriptor: public ThreadDescriptorBase
{
    friend class ThreadManager;
    friend class DoubleLinkedList<ThreadDescriptor>;
    friend class DoubleLinkedListIterator<ThreadDescriptor>;
    
public:
    ThreadDescriptor( void );
    ~ThreadDescriptor( void );
    
    thread_t GetSelfThreadId( void );
    
    void Acquire( void );
    void Release( void );
    //void AcquireRelease( void );
    
    void SetNext( ThreadDescriptor *thrDesc );
    ThreadDescriptor *GetNext( void ) const;
    
private:
    void Reset( ThreadManager *thrMgr );
    
    ThreadManager *m_ThrMgr;
    
    DEFAULT_THREAD_MANAGER_LOCK *m_Synch;
    
    bool m_IsTerminated;
};

class ThreadAdapterBase
{
protected:
    ThreadAdapterBase( THR_FUNC UserFunc
                        , void *args
                        , THR_C_FUNC EntryPoint = (THR_C_FUNC) THREAD_ADAPTER_NAME
                        , OSThreadDescriptor *thrDesc = 0
                        , long lCancelFlags = 0 );

public:
    virtual ~ThreadAdapterBase( void ) { }
    
    virtual void* Invoke( void ) = 0;

    THR_C_FUNC GetEntryPoint( void );
    
protected:
    THR_FUNC mfp_UserFunc;
    
    void *m_Args;
    
    THR_C_FUNC mfp_EntryPoint;
    
    OSThreadDescriptor *m_OSThrDesc;
    
    long m_CancelFlags;
};

class OSThreadAdapter: public ThreadAdapterBase
{
public:
    OSThreadAdapter( THR_FUNC UserFunc
                    , void *args
                    , THR_C_FUNC EntryPoint = (THR_C_FUNC)THREAD_ADAPTER_NAME
                    , long cancelFlags = 0 );

    virtual THR_FUNC_RETURN Invoke( void ){ return NULL; }

protected:
    virtual ~OSThreadAdapter( void ) { }
};

class ThreadAdapter: public ThreadAdapterBase
{
public:
    ThreadAdapter( THR_FUNC UserFunc
                    , void *args
                    , THR_C_FUNC EntryPoint = (THR_C_FUNC) THREAD_ADAPTER_NAME
                    , ThreadManager *thrMgr = 0
                    , ThreadDescriptor *thrDesc = 0
                    , long cancelFlags = 0 );
    
    virtual THR_FUNC_RETURN Invoke( void );
    
    ThreadManager *GetThreadMgr( void );

protected:
    ~ThreadAdapter( void ) { }

private:
    virtual THR_FUNC_RETURN InvokeAux( void );
    
    ThreadManager *m_ThrMgr;
};

#endif

