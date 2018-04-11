#ifndef __SOCK_ACCEPTOR_H__
#define __SOCK_ACCEPTOR_H__

#include "Public.h"
#include "BaseAddress.h"
#include "TimeValue.h"
#include "SockStream.h"

class SockAcceptor: public SockBase
{
public:
    SockAcceptor( void ) {}
    ~SockAcceptor( void ) {}

    SockAcceptor( const BaseAddress &localAddr
                , int iReuseAddr = 0
                , int iProtocolFamily = PF_UNSPEC
                , int iBackLog = DEFAULT_BACKLOG
                , int iProtocol = 0 );

    int Open( const BaseAddress &localAddr
            , int iReuseAddr = 0
            , int iProtocolFamily = PF_UNSPEC
            , int iBackLog = DEFAULT_BACKLOG
            , int iProtocol = 0 );

    int Accept( SockStream &newSockStream
                , BaseAddress *remoteAddr = 0
                , TimeValue *timeOut = 0
                , bool bRestart = true
                , bool bResetNewHandle = false );

    int Close( void );
protected:
    int HandleTimedAccept( HANDLE listener, TimeValue *timeOut, bool bRestart ) const;
    int SharedAcceptStart( TimeValue *timeOut
                            , bool bRestart
                            , int &iBlockingMode );

    int SharedAcceptFinish( SockStream newSockStream
                            , int iBlockingMode );

    int SharedOpen( const BaseAddress &localAddr
                        , int iProtocolFamily
                        , int iBacklog );

private:
    int GetRemoteAddress(BaseAddress & addr) const;
};

#endif

