#include "SockAcceptor.h"
#include "InetAddress.h"

SockAcceptor::SockAcceptor(const BaseAddress &localAddr
                           , int iReuseAddr
                           , int iProtocolFamily
                           , int iBackLog
                           , int iProtocol)
{
    if(-1 == this->Open(localAddr, iReuseAddr, iProtocolFamily, iBackLog, iProtocol))
    {
        cout << "SockAcceptor ctr failed!" << endl;
    }
}

int SockAcceptor::Open(const BaseAddress &localAddr
                       , int iReuseAddr
                       , int iProtocolFamily
                       , int iBackLog
                       , int iProtocol)
{
    if(localAddr != BaseAddress::addrAny)
    {
        iProtocolFamily = localAddr.GetAddrType();
    }
    else if(PF_UNSPEC == iProtocolFamily)
    {
#ifdef HAS_IPV6
        iProtocolFamily = InetAddress::IPv6IsEnabled() ? PF_INET6 : PF_INET;
#else
        iProtocolFamily = PF_INET;
#endif
    }
    if(-1 == SockBase::Open(SOCK_STREAM
                            , iProtocolFamily
                            , iProtocol
                            , iReuseAddr))
    {
        cout<<"SockAcceptor::Open SockBase::Open failed!"<<endl;
        return -1;
    }
    return this->SharedOpen(localAddr, iProtocolFamily, iBackLog);
}

int SockAcceptor::Accept( SockStream &newSockStream
                , BaseAddress *remoteAddr
                , TimeValue *timeOut
                , bool bRestart
                , bool bResetNewHandle )
{
    int iBlockingMode = 0;

    if( -1 == this->SharedAcceptStart(timeOut, bRestart, iBlockingMode) )
    {
        return -1;
    }

    socklen_t *ptrAddrLen = NULL;
    sockaddr *cltAddr = NULL;
    int iAddrLen = 0;

    if( remoteAddr != NULL )
    {
        iAddrLen = remoteAddr->GetAddrSize();
        ptrAddrLen = reinterpret_cast<socklen_t*>( &iAddrLen );
        cltAddr = static_cast<sockaddr*>( remoteAddr->GetAddress() );
    }

    do
    {
        // the third parameter to <accept> must be a NULL
        // pointer if we want to ignore the client's address.
        newSockStream.SetHandle( accept(this->GetHandle(), cltAddr, ptrAddrLen) );
    }
    while( newSockStream.GetHandle() != INVALID_HANDLE
            && bRestart
            && errno == EINTR
            && NULL == timeOut );

    if( newSockStream.GetHandle() != INVALID_HANDLE && remoteAddr != NULL )
    {
        remoteAddr->SetAddrSize( iAddrLen );
        if( cltAddr != NULL )
        {
            remoteAddr->SetAddrType( cltAddr->sa_family );
        }
    }

    return this->SharedAcceptFinish( newSockStream, iBlockingMode );
}

int SockAcceptor::Close(void)
{
    return SockBase::CloseSocket();
}

int SockAcceptor::SharedOpen(const BaseAddress & localAddr
                             , int iProtocolFamily
                             , int iBacklog)
{
    int iError = 0;

#ifdef HAS_IPV6
    if(PF_INET6 == iProtocolFamily)
    {
        sockaddr_in6 localInet6Addr;
        memset(&localInet6Addr, 0x00, sizeof(localInet6Addr));

        if(BaseAddress::addrAny == localAddr)
        {
            localInet6Addr.sin6_family = AF_INET6;
            localInet6Addr.sin6_port = 0;
            localInet6Addr.sin6_addr = in6addr_any; //IN6ADDR_ANY_INIT
        }
        else
        {
            localInet6Addr = *reinterpret_cast<sockaddr_in6*>(localAddr.GetAddress());
        }
        if(-1 == bind(this->GetHandle(), reinterpret_cast<sockaddr*>(&localInet6Addr), sizeof(localInet6Addr)))
        {
            iError = 1;
        }
    }
    else
#endif
    {
        if(PF_INET == iProtocolFamily)
        {
            if(-1 == bind(this->GetHandle()
                          , reinterpret_cast<sockaddr*>(localAddr.GetAddress())
                          , localAddr.GetAddrSize()))
            {
                cout<<"SockAcceptor::SharedOpen bind failed!"<<endl;
                iError = 1;
            }
        }
    }
    if(iError != 0 || -1 == listen(this->GetHandle(), iBacklog))
    {
        cout<<"SockAcceptor::SharedOpen listen failed!"<<endl;
        iError = 1;
        this->Close();
    }
    return iError ? -1 : 0;
}

int SockAcceptor::HandleTimedAccept(HANDLE listener, TimeValue *timeOut, bool bRestart) const
{
    if(INVALID_HANDLE == listener)
    {
        return -1;
    }

    struct pollfd poFds;
    poFds.fd = listener;
    poFds.events = POLLIN;
    poFds.revents = 0;

    for(;;)
    {
        cout<<"SockAcceptor::HandleTimedAccept call_poll begin"<<endl;
        int iRet = poll( &poFds, 1, timeOut->GetMilliSecond() );
        cout<<"SockAcceptor::HandleTimedAccept call_poll iRet="<<iRet<<endl;

        switch(iRet)
        {
            case -1:
                if(EINTR == errno && bRestart)
                {
                    cout<<"SockAcceptor::HandleTimedAccept call_poll errno=EINTR"<<endl;
                    continue;
                }
                else
                {
                    cout<<"SockAcceptor::HandleTimedAccept call_poll errno="<<errno<<endl;
                    return -1;
                }
            case 0:
                if( timeOut != NULL && TimeValue::zero == *timeOut )
                {
                    cout<<"SockAcceptor::HandleTimedAccept call_poll set errno=EWOULDBLOCK="<<EWOULDBLOCK<<endl;
                    errno = EWOULDBLOCK;
                }
                else
                {
                    cout<<"SockAcceptor::HandleTimedAccept call_poll set errno=ETIMEDOUT="<<ETIMEDOUT<<endl;
                    errno = ETIMEDOUT;
                }
                return -1;
            case 1:
                return 0;
            default:
                errno = EINVAL;
                return -1;
        }
    }
}

int SockAcceptor::SharedAcceptStart(TimeValue *timeOut
                                    , bool bRestart
                                    , int &iBlockingMode)
{
    HANDLE listener = this->GetHandle();

    if( timeOut != NULL )
    {
        if( -1 == this->HandleTimedAccept(listener, timeOut, bRestart) )
        {
            return -1;
        }
        iBlockingMode = BIT_DISABLED( listener, O_NONBLOCK );
        if( iBlockingMode && -1 == this->SetSingleFlags( O_NONBLOCK ) )
        {
            return -1;
        }
    }
    return 0;
}

int SockAcceptor::SharedAcceptFinish( SockStream newSockStream
                        , int iBlockingMode )
{
    HANDLE newHandle = newSockStream.GetHandle();

    if( iBlockingMode )
    {
        HANDLE acceptHandle = this->GetHandle();
        CLR_BITS( acceptHandle, O_NONBLOCK );
        this->SetHandle( acceptHandle );
        
        CLR_BITS( newHandle, O_NONBLOCK );
    }
    return newHandle == INVALID_HANDLE ? -1 : 0;
}


