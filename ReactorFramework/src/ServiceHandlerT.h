#ifndef __SERVICE_HANDLER_H__
#define __SERVICE_HANDLER_H__

#include "Public.h"
#include "TimeValue.h"
#include "ThreadManager.h"
#include "MessageQueue.h"
#include "ConnectionRecyclingStrategy.h"
#include "Reactor.h"

class ConnectionRecyclingStrategy;

template<class PEER_STREAM, class SYNCH_TRAITS>
class ServiceHandler: public Task<SYNCH_TRAITS>
{
public:
    typedef typename PEER_STREAM::PEER_ADDR AddrType;
    typedef PEER_STREAM StreamType;

    ServiceHandler( ThreadManager *thrMgr = NULL
                    , MessageQueue<SYNCH_TRAITS> *msgQueue = NULL
                    , Reactor *reactor = Reactor::GetInstance() );

    virtual ~ServiceHandler( void );

    virtual int Open( void *accOrConn = NULL );
    virtual int Close( ulong lFlag = 0 );

    virtual int HandleClose( HANDLE = INVALID_HANDLE, ReactorMask = EventHandler::ALL_EVENTS_MASK );
    virtual int HandleTimeout( const TimeValue &timeout, const void * );

    virtual HANDLE GetHandle( void ) const;
    virtual void SetHandle( HANDLE );

    virtual ConnectionRecyclingStrategy* GetRecycler( void ) const;
    virtual void SetRecycler( ConnectionRecyclingStrategy *recycler, const void *recyclingAct );
    virtual int Recycle( void * = NULL );

    PEER_STREAM & GetPeer( void );
    //Reactor* GetReactor( void ) const;
    //void SetReactor( Reactor *reactor );

    void* operator new( size_t size );
    void* operator new( size_t size, std::nothrow_t & ) throw();
    void* operator new( size_t size, void *p );
    void operator delete( void *p, std::nothrow_t & ) throw();
    void operator delete (void *);
    void operator delete (void *, void *);

    virtual void Destroy( void );

    void Shutdown( void );

protected:
    PEER_STREAM                 m_Peer;
    bool                        m_bIsDynamic;
    bool                        m_bClosing;
    ConnectionRecyclingStrategy *m_pRecycler;
    const void                  *m_pRecyclingAct;

    //Reactor                     *m_pReactor;
};

template<class PEER_STREAM, class SYNCH_TRAITS>
ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::ServiceHandler( ThreadManager *thrMgr
                                                            , MessageQueue<SYNCH_TRAITS> *msgQueue
                                                            , Reactor *reactor )
    : Task<SYNCH_TRAITS>( thrMgr, msgQueue )
    , m_bIsClosing( false )
    , m_pRecycler( NULL )
    , m_pRecyclingAct( NULL )
{
    this->SetReactor( reactor );
}

template<class PEER_STREAM, class SYNCH_TRAITS>
ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::~ServiceHandler( void )
{
    if( !this->m_bIsClosing )
    {
        this->m_bIsClosing = true;
        this->Shutdown();
    }
}

template <typename PEER_STREAM, typename SYNCH_TRAITS>
int ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::Open( void *accOrConn )
{
    if( this->GetReactor() && this->GetReactor()->RegisterHandler(this, EventHandler::READ_MASK) == -1 )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"ServiceHandler::Open unable to register client handler"<<endl;
    }
    return 0;
}

template <typename PEER_STREAM, typename SYNCH_TRAITS>
int ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::Close( ulong lFlag )
{
    this->HandleClose();
}

template <typename PEER_STREAM, typename SYNCH_TRAITS>
int ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::HandleClose( HANDLE, ReactorMask )
{
    if( this->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::DISABLED )
    {
        this->Destroy();
    }

    return 0;
}

template <typename PEER_STREAM, typename SYNCH_TRAITS>
int ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::HandleTimeout( const TimeValue &timeout, const void * )
{
    return this->HandleClose();
}

template <typename PEER_STREAM, typename SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::Shutdown( void )
{
    // Deregister this handler with the ACE_Reactor.
    if( this->GetReactor() )
    {
        ReactorMask mask = EventHandler::ALL_EVENTS_MASK | EventHandler::DONT_CALL;

        // Make sure there are no timers.
        this->GetReactor()->CancelTimer( this );

        if( this->GetPeer().GetHandle() != INVALID_HANDLE )
        {
            // Remove self from reactor.
            this->GetReactor()->RemoveHandler( this, mask );
        }
    }

    // Remove self from the recycler.
    if( this->GetRecycler() )
    {
        this->GetRecycler()->Purge( this->m_pRecyclingAct );
    }

    this->GetPeer().Close();
}

template<class PEER_STREAM, class SYNCH_TRAITS>
HANDLE ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::GetHandle( void ) const
{
    return this->m_Peer.GetHandle();
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::SetHandle( HANDLE handle )
{
    this->m_Peer.SetHandle( handle );
}

template<class PEER_STREAM, class SYNCH_TRAITS>
ConnectionRecyclingStrategy* ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::GetRecycler( void ) const
{
    return this->m_pRecycler;
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::SetRecycler( ConnectionRecyclingStrategy *recycler, const void *recyclingAct )
{
    this->m_pRecycler = recycler;
    this->m_pRecyclingAct = recyclingAct;
}
template<class PEER_STREAM, class SYNCH_TRAITS>
int ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::Recycle( void * )
{
    return 0;
}

template<class PEER_STREAM, class SYNCH_TRAITS>
PEER_STREAM & ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::GetPeer( void )
{
    return (PEER_STREAM &)this->m_Peer;
}

/*template<class PEER_STREAM, class SYNCH_TRAITS>
Reactor* ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::GetReactor( void ) const
{
    return this->m_pReactor;
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::SetReactor( Reactor *reactor )
{
    this->m_pReactor = reactor;
}*/

template<class PEER_STREAM, class SYNCH_TRAITS>
void* ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator new( size_t size )
{
    Dynamic *const dynamic_instance = Dynamic::GetInstance();

    if( NULL == dynamic_instance )
    {
        throw std::bad_alloc;
    }
    else
    {
        dynamic_instance->SetFlag();

        return new char[size];
    }
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void* ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator new( size_t size, std::nothrow_t & ) throw()
{
    Dynamic *const instance = Dynamic::GetInstance();

    if( NULL == instance )
    {
        return 0;
    }
    else
    {
        instance->SetFlag();

        return new(std::nothrow) char[size];
    }
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void* ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator new( size_t size, void *p )
{
    return p;
}

template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator delete( void *p, std::nothrow_t & ) throw()
{
    delete[] static_cast<char*>( p );
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator delete( void * )
{
    if( this->mod_ == 0 && this->m_bIsDynamic && this->m_bClosing == false )
    {
        delete this;
    }
}
template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::operator delete( void *, void * )
{
    return;
}

template<class PEER_STREAM, class SYNCH_TRAITS>
void ServiceHandler<PEER_STREAM, SYNCH_TRAITS>::Destroy( void )
{
    if( this->m_bIsDynamic && this->m_bClosing )
    {
        delete this;
    }
}

#endif

