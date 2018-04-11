#ifndef __SERVER_ACCEPTOR_H__
#define __SERVER_ACCEPTOR_H__

#include "Public.h"
#include "InetAddress.h"
#include "Reactor.h"
#include "EventHandler.h"
#include "SockAcceptor.h"
#include "SockStream.h"

class ServerAcceptor: public EventHandler
{
public:
    typedef InetAddress PEER_ADDR;

    ServerAcceptor( Reactor *reactor = Reactor::GetInstance() ): EventHandler( reactor )
    {
        cout<<"ServerAcceptor ctor"<<endl;
    }

    virtual int Open( const InetAddress &localAddr );

    virtual int HandleInput( HANDLE handle = INVALID_HANDLE );

    virtual int HandleClose( HANDLE handle = INVALID_HANDLE, ReactorMask mask = 0 );

    virtual HANDLE GetHandle( void ) const;

    SockAcceptor GetAcceptor( void ) const;

protected:
    ~ServerAcceptor( void ) {}

protected:
    SockAcceptor m_soAcceptor;
};

#endif

