#include "SockBase.h"

SockBase::SockBase( void )
{
}

int SockBase::CloseSocket( void )
{
    int iRet = 0;
    
    if( this->GetHandle() != INVALID_HANDLE )
    {
        iRet = close( this->GetHandle() );
        this->SetHandle( INVALID_HANDLE );
    }
    return iRet;
}

int SockBase::Open( int iType, int iProtocolFamily, int iProtocol, bool bIsReuseAddr )
{
    this->SetHandle( socket(iProtocolFamily, iType, iProtocol) );
    
    if( INVALID_HANDLE == this->GetHandle() )
    {
        cout<<"SockBase::Open call_socket failed!"<<endl;
        return -1;
    }
    if( PF_UNIX != iProtocolFamily
        && bIsReuseAddr
        && -1 == this->SetSockOption( SOL_SOCKET, SO_REUSEADDR, &bIsReuseAddr, sizeof(bIsReuseAddr) ) )
    {
        cout<<"SockBase::Open call_SetSockOption failed!"<<endl;
        this->CloseSocket();
        return -1;
    }
    
    return 0;
}

SockBase::SockBase( int iType, int iProtocolFamily, int iProtocol, bool bIsReuseAddr )
{
    this->Open( iType, iProtocolFamily, iProtocol, bIsReuseAddr );
}

int SockBase::GetLocalAddress( BaseAddress &addr ) const
{
    socklen_t iAddrLen = addr.GetAddrSize();
    struct sockaddr *soAddr = reinterpret_cast<struct sockaddr*>( addr.GetAddress() );
    
    if( -1 == getsockname(this->GetHandle(), soAddr, &iAddrLen) )
    {
        return -1;
    }
    
    addr.SetAddrType( soAddr->sa_family );
    addr.SetAddrSize( iAddrLen );
    
    return 0;
}

int SockBase::GetRemoteAddress( BaseAddress &addr ) const
{
    socklen_t iAddrLen = addr.GetAddrSize();
    struct sockaddr *soAddr = reinterpret_cast<struct sockaddr*>( addr.GetAddress() );
    
    if( -1 == getpeername(this->GetHandle(), soAddr, &iAddrLen ) )
    {
        return -1;
    }
    
    addr.SetAddrType( soAddr->sa_family );
    addr.SetAddrSize( iAddrLen );
    
    return 0;
}

