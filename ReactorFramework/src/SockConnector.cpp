#include "SockConnector.h"

int SockConnector::SharedOpen( SockStream &soStreamNew, int iProtocolFamily, int iProtocol, bool bReuseAddr )
{
    if( INVALID_HANDLE == soStreamNew.GetHandle()
        && -1 == soStreamNew.Open( SOCK_STREAM, iProtocolFamily, iProtocol, bReuseAddr ) )
    {
        cout<<"SockConnector::SharedOpen call_soStreamNew.Open failed!"<<endl;
        return -1;
    }
    return 0;
}

int SockConnector::HandleTimedConnect( HANDLE handle, const TimeValue *timeout, bool bIsTLI )
{
    struct pollfd pollFds;
    
    pollFds.fd = handle;
    //pollFds.events = POLLIN | POLLOUT;
    pollFds.events = POLLOUT;   //connFd 只需判断是否可写
    pollFds.revents = 0;
    
    bool bNeedToCheck = false;
    bool bKnownFailure = false;
    
    int iMilliSecond = NULL == timeout ? -1: timeout->GetMilliSecond();
    cout<<"SockConnector::HandleTimedConnect iMilliSecond="<<iMilliSecond<<endl;
    
    cout<<"SockConnector::HandleTimedConnect call_poll begin!"<<endl;
    int iEvents = poll( &pollFds, 1, iMilliSecond );
    if( iEvents <= 0 )
    {
        if( 0 == iEvents && timeout != NULL )
        {
            cout<<"SockConnector::HandleTimedConnect call_poll timeout"<<endl;
            errno = ETIME;
            return -1;
        }
    }
    if( bIsTLI )
    {
        bNeedToCheck = ( pollFds.events & POLLIN ) && !( pollFds.events & POLLOUT );
    }
    else
    {
        bNeedToCheck = ( pollFds.events & POLLIN ) || ( pollFds.events & POLLERR );
        bKnownFailure = pollFds.events & POLLERR;
    }
    
    cout<<"SockConnector::HandleTimedConnect call_poll bNeedToCheck="<<bNeedToCheck<<endl;
    if( bNeedToCheck )
    {
        int iSockErr = 0;
        socklen_t iErrLen = sizeof( iSockErr );
        int iRet = getsockopt( handle, SOL_SOCKET, SO_ERROR, (char*)&iSockErr, &iErrLen );
        cout<<"SockConnector::HandleTimedConnect call_getsockopt iRet="<<iRet<<"__iSockErr="<<iSockErr<<"__errno="<<errno<<endl;
        if( iRet < 0 )
        {
            return -1;
        }
        if( iSockErr != 0 )
        {
            errno = iSockErr;
            return -1;
        }
        #if 0
        char cDummy;
        int iRecvLen = recv( handle, &cDummy, 1, MSG_PEEK );
        cout<<"SockConnector::HandleTimedConnect call_recv iRecvLen="<<iRecvLen<<"__errno="<<errno<<endl;
        if( iRecvLen <= 0 )
        {
            if( 0 == iRecvLen )
            {
                cout<<"SockConnector::HandleTimedConnect nonblock connect refused!"<<endl;
                errno = ECONNREFUSED;
                return -1;
            }
            else if( EWOULDBLOCK != errno && EAGAIN != errno )
            {
                return -1;
            }
        }
        #endif
    }
    return 0;
}

int SockConnector::Complete( SockStream &soStreamNew
                            , BaseAddress *addrRemote
                            , const TimeValue *timeout )
{
    if( -1 == this->HandleTimedConnect( soStreamNew.GetHandle(), timeout ) )
    {
        soStreamNew.CloseSocket();
        return -1;
    }
    
    if( addrRemote != NULL )
    {
        socklen_t iAddrLen = addrRemote->GetAddrSize();
        sockaddr *addr = reinterpret_cast<sockaddr*>( addrRemote->GetAddress() );
        if( -1 == getpeername( soStreamNew.GetHandle(), addr, &iAddrLen ) )
        {
            soStreamNew.CloseSocket();
            return -1;
        }
    }
    
    soStreamNew.Disable( O_NONBLOCK );
    
    return 0;
}

int SockConnector::SharedConnectStart( SockStream &soStreamNew
                            , const TimeValue *timeout
                            , const BaseAddress &addrLocal )
{
    if( addrLocal != BaseAddress::addrAny )
    {
        sockaddr *soAddrLocal = reinterpret_cast<sockaddr*>( addrLocal.GetAddress() );
        int iAddrLen = addrLocal.GetAddrSize();
        
        if( -1 == bind(soStreamNew.GetHandle(), soAddrLocal, iAddrLen) )
        {
            soStreamNew.CloseSocket();
            return -1;
        }
    }
    
    if( timeout != NULL && -1 == soStreamNew.Enable(O_NONBLOCK) )
    {
        soStreamNew.CloseSocket();
        return -1;
    }
    return 0;
}

int SockConnector::SharedConnectFinish( SockStream &soStreamNew
                            , const TimeValue *timeout
                            , int iResult )
{
    int iError = errno;
    cout<<"SockConnector::SharedConnectFinish errno="<<errno<<"__EINPROGRESS="<<EINPROGRESS<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;

    if( -1 == iResult && timeout != NULL )
    {
        if( EINPROGRESS == iError || EWOULDBLOCK == iError )
        {
            if( TimeValue::zero == *timeout )
            {
                iError = EWOULDBLOCK;
            }
            else if( -1 == this->Complete( soStreamNew, 0, timeout ) )
            {
                iError = errno;
            }
            else
            {
                return 0;
            }
        }
    }
    
    if( iResult != -1 || iError == EISCONN )
    {
        if( -1 == ( iResult = soStreamNew.Disable(O_NONBLOCK) ) )
        {
            soStreamNew.CloseSocket();
        }
    }
    else if( !( EWOULDBLOCK == iError || ETIME == iError ) )
    {
        soStreamNew.CloseSocket();
    }
    
    return iResult;
}

int SockConnector::Connect( SockStream &soStreamNew
                , const BaseAddress &addrRemote
                , const TimeValue *timeout
                , const BaseAddress &addrLocal
                , bool bReuseAddr
                , int iFlags
                , int iPerms
                , int iProtocol )
{
    if( -1 == this->SharedOpen(soStreamNew, addrRemote.GetAddrType(), iProtocol, bReuseAddr) )
    {
        cout<<"SockConnector::Connect call_SharedOpen failed"<<endl;
        return -1;
    }
    else if( -1 == this->SharedConnectStart(soStreamNew, timeout, addrLocal) )
    {
        cout<<"SockConnector::Connect call_SharedConnectStart failed"<<endl;
        return -1;
    }
    
    int iRet = connect( soStreamNew.GetHandle()
                    , static_cast<sockaddr*>( addrRemote.GetAddress() )
                    , addrRemote.GetAddrSize() );
    cout<<"SockConnector::Connect call_connect iRet="<<iRet<<"__errno="<<errno<<endl;

    iRet = this->SharedConnectFinish( soStreamNew, timeout, iRet );
    cout<<"SockConnector::Connect call_SharedConnectFinish iRet="<<iRet<<endl;

    return iRet;
}

SockConnector::SockConnector( SockStream &soStreamNew
                            , const BaseAddress &addrRemote
                            , const TimeValue *timeout
                            , const BaseAddress &addrLocal
                            , bool bReuseAddr
                            , int iFlags
                            , int iPerms
                            , int iProtocol )
{
    if( -1 == this->Connect( soStreamNew, addrRemote, timeout, addrLocal, bReuseAddr, iFlags, iPerms, iProtocol ) )
    {
        cout<<"SockConnector constructor failed"<<endl;
    }
}
