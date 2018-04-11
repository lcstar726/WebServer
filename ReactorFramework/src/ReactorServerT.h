#ifndef __REACTOR_SERVER_H__
#define __REACTOR_SERVER_H__

#include "Public.h"
#include "Reactor.h"

template<class ACCEPTOR>
class ReactorServerT: public ACCEPTOR
{
public:
    ReactorServerT( int argc, char* argv[], Reactor *reactor );

private:
    typename ACCEPTOR::PEER_ADDR    m_SvrAddress;

    uint16  m_iSvrPort;
};

template<class ACCEPTOR>
ReactorServerT<ACCEPTOR>::ReactorServerT( int argc, char* argv[], Reactor *reactor ): ACCEPTOR( reactor )
{
    this->m_iSvrPort = 23456;
    int iRet = 0;

    iRet = this->m_SvrAddress.SetInetAddr( m_iSvrPort, (uint32)INADDR_ANY );

    if( iRet != -1 )
    {
        iRet = ACCEPTOR::Open( this->m_SvrAddress );
    }
    if( iRet == -1 )
    {
        reactor->EndReactorEventLoop();
    }
}

#endif

