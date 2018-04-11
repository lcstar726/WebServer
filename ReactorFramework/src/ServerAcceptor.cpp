#include "ServerAcceptor.h"
#include "RequestEventHandler.h"

inline HANDLE ServerAcceptor::GetHandle( void ) const
{
    return m_soAcceptor.GetHandle();
}

SockAcceptor ServerAcceptor::GetAcceptor( void ) const
{
    return this->m_soAcceptor;
}

int ServerAcceptor::Open( const InetAddress &localAddr )
{
    if( -1 == this->m_soAcceptor.Open(localAddr) )
    {
        return -1;
    }

    return this->GetReactor()->RegisterHandler( this, EventHandler::ACCEPT_MASK );
}

int ServerAcceptor::HandleInput( HANDLE handle )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__ServerAcceptor::HandleInput handle="<<handle<<endl;

    RequestEventHandler *requestEvtHandler;

    NEW_RETURN( requestEvtHandler, RequestEventHandler( this->GetReactor() ), -1 );

    if( -1 == this->m_soAcceptor.Accept( requestEvtHandler->GetClientPeer() ) )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__ServerAcceptor::HandleInput m_soAcceptor.Accept failed!"<<endl;
        delete requestEvtHandler;
        return -1;
    }
    else if( -1 == requestEvtHandler->Open() )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__ServerAcceptor::HandleInput m_soAcceptor.Open failed!"<<endl;
        requestEvtHandler->HandleClose();

        return -1;
    }

    return 0;
}

int ServerAcceptor::HandleClose( HANDLE, ReactorMask )
{
    this->m_soAcceptor.Close();

    delete this;

    return 0;
}

