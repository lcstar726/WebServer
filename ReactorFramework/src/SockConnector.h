#ifndef __SOCK_CONNECTOR_H__
#define __SOCK_CONNECTOR_H__

#include "InetAddress.h"
#include "SockStream.h"

class SockConnector
{
public:
    SockConnector( void );
    SockConnector( SockStream &soStreamNew
                , const BaseAddress &addrRemote
                , const TimeValue *timeout = NULL
                , const BaseAddress &addrLocal = BaseAddress::addrAny
                , bool bReuseAddr = false
                , int iFlags = 0
                , int iPerms = 0
                , int iProtocol = 0 );
    ~SockConnector( void );

    int Connect( SockStream &soStreamNew
            , const BaseAddress &addrRemote
            , const TimeValue *timeout = NULL
            , const BaseAddress &addrLocal = BaseAddress::addrAny
            , bool bReuseAddr = false
            , int iFlags = 0
            , int iPerms = 0
            , int iProtocol = 0 );

    typedef InetAddress PeerAddress;
    typedef SockStream  PeerStream;

protected:
    int HandleTimedConnect( HANDLE handle, const TimeValue *timeout, bool bIsTLI = false );
    
    int Complete( SockStream &soStreamNew
                , BaseAddress *addrRemote = NULL
                , const TimeValue *timeout = NULL );

    int SharedOpen( SockStream &soStreamNew
                    , int iProtocolFamily
                    , int iProtocol
                    , bool bReuseAddr );

    int SharedConnectStart( SockStream &soStreamNew
                            , const TimeValue *timeout
                            , const BaseAddress &addrLocal );
    int SharedConnectFinish( SockStream &soStreamNew
                            , const TimeValue *timeout
                            , int iResult );
};

inline SockConnector::SockConnector( void ) {}
inline SockConnector::~SockConnector( void ) {}

#endif
