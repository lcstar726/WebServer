#include "InetAddress.h"

void* InetAddress::GetAddress( void ) const
{
    return (void*)&(this->m_uInetAddr);
}

bool InetAddress::IPv6IsEnabled( void )
{
    bool bIsEnabled = false;

#ifdef HAS_IPV6
    int sockFd = socket( PF_INET6, SOCK_DGRAM, 0 );
    if( -1 != sockFd )
    {
        bIsEnabled = true;
        close( sockFd );
    }
#endif
    
    return bIsEnabled;
}

InetAddress::InetAddress( void )
    :BaseAddress( DetermineType(), sizeof(m_uInetAddr) )
{
    this->Reset();
}

InetAddress::~InetAddress( void )
{
}

int InetAddress::SetInetAddr( const InetAddress &addr )
{
    if( AF_ANY == addr.GetAddrType() )
    {
        memset( &this->m_uInetAddr, 0x00, sizeof(this->m_uInetAddr) );
    }
    else
    {
        this->SetAddrType( addr.GetAddrType() );
        this->SetAddrSize( addr.GetAddrSize() );
        
        memcpy( &this->m_uInetAddr, &addr.m_uInetAddr, addr.GetAddrSize() );
    }
    
    return 0;
}
InetAddress::InetAddress( const InetAddress &addr ): BaseAddress( addr.GetAddrType(), addr.GetAddrSize() )
{
    this->Reset();
    this->SetInetAddr( addr );
}

void InetAddress::SetPortNumber( uint16 iPortNum, bool bNeedEncode )
{
    if( bNeedEncode )
    {
        iPortNum = htons( iPortNum );
    }
    if( AF_INET == this->GetAddrType() )
    {
        this->m_uInetAddr.m_AddrIN4.sin_port = iPortNum;
    }
    else
    {
#ifdef HAS_IPV6
        this->m_uInetAddr.m_AddrIN6.sin6_port = iPortNum;
#endif
    }
}

void InetAddress::SetAddress( void *addr, int iLen, int iMap )
{
    struct sockaddr_in *addrV4 = static_cast<struct sockaddr_in*>( addr );
    if( AF_INET == addrV4->sin_family )
    {
#ifdef HAS_IPV6
        if( iMap )
        {
            this->SetAddrType( AF_INET6 );
        }
        else
#endif
        {
            this->SetAddrType( AF_INET );
        }
        this->SetPortNumber( addrV4->sin_port, 0 );
        this->SetInetAddr( reinterpret_cast<const char*>(&addrV4->sin_addr), sizeof(addrV4->sin_addr), 0, iMap );
    }
#ifdef HAS_IPV6
    else if( AF_INET6 == addrV4->sin_family )
    {
        struct sockaddr_in6 *addrV6 = static_cast<struct sockaddr_in6*>( addr );
        this->SetAddrType( AF_INET6 );
        this->SetPortNumber( addrV6->sin6_port, 0 );
        this->SetInetAddr( reinterpret_cast<const char*>(&addrV6->sin6_addr), sizeof(addrV6->sin6_addr), 0 );
        this->m_uInetAddr.m_AddrIN6.sin6_scope_id = addrV6->sin6_scope_id;
    }
#endif
}
void InetAddress::SetAddress(  void *addr, int iLen )
{
    this->SetAddress( addr, iLen, 0 );
}

int InetAddress::SetInetAddr( const char *SAddr, int iAddrLen, bool bNeedEncode, int iMap )
{
    if( bNeedEncode && iAddrLen != 4 )
    {
        errno = EAFNOSUPPORT;
        return -1;
    }
    
    if( 4 == iAddrLen )
    {
        uint32 iSAddrV4 = *reinterpret_cast<const uint32*>( SAddr );
        if( bNeedEncode )
        {
            iSAddrV4 = htonl( iSAddrV4 );
        }
        if( AF_INET == this->GetAddrType() && 0 == iMap )
        {
            this->BaseSet( AF_INET, sizeof(this->m_uInetAddr.m_AddrIN4) );
            this->m_uInetAddr.m_AddrIN4.sin_family = AF_INET;
            memcpy( &(this->m_uInetAddr.m_AddrIN4.sin_addr), &iSAddrV4, iAddrLen );
        }
        
        // If given an IPv4 address to copy to an IPv6 object, map it to
        // an IPv4-mapped IPv6 address.
#ifdef HAS_IPV6
        if( iMap != 0 )
        {
            this->BaseSet( AF_INET6, sizeof(this->m_uInetAddr.m_AddrIN6) );
            this->m_uInetAddr.m_AddrIN6.sin6_family = AF_INET6;
            // Build up a 128 bit address.  An IPv4-mapped IPv6 address
            // is defined as 0:0:0:0:0:ffff:IPv4_address.  This is defined
            // in RFC 1884 */
            memset( &(this->m_uInetAddr.m_AddrIN6.sin6_addr), 0x00, 16 );
            this->m_uInetAddr.m_AddrIN6.sin6_addr.s6_addr[10] = this->m_uInetAddr.m_AddrIN6.sin6_addr.s6_addr[11] = 0xff;
            memcpy( &(this->m_uInetAddr.m_AddrIN6.sin6_addr), &iSAddrV4, iAddrLen );
        }
#endif

        return 0;
    }
#ifdef HAS_IPV6
    else if( 16 == iAddrLen )
    {
        if( AF_INET6 != this->GetAddrType() )
        {
            errno = EAFNOSUPPORT;
            return -1;
        }
        this->BaseSet( AF_INET6, sizeof(this->m_uInetAddr.m_AddrIN6) );
        this->m_uInetAddr.m_AddrIN6.sin6_family = AF_INET6;
        memcpy( &(this->m_uInetAddr.m_AddrIN6), SAddr, iAddrLen );
        return 0;
    }
#endif
    
    errno = EAFNOSUPPORT;
    return -1;
}

int InetAddress::SetInetAddr( uint16 iPort, uint32 iSAddr, bool bNeedEncode, int iMap )
{
    this->SetInetAddr( reinterpret_cast<const char*>(&iSAddr), sizeof(iSAddr), bNeedEncode, iMap );
    
    this->SetPortNumber( iPort, bNeedEncode );
    
    return 0;
}

int InetAddress::SetInetAddr( uint16 iPort, const char *cstrHostName, bool bNeedEncode, int iAddrFamily )
{
    if( 0 == cstrHostName )
    {
        errno = EINVAL;
        return -1;
    }
    memset( &this->m_uInetAddr, 0x00, sizeof(this->m_uInetAddr) );
    
#ifdef HAS_IPV6
    if( AF_UNSPEC == iAddrFamily && !IPv6IsEnabled() )
    {
        iAddrFamily = AF_INET;
    }
    if( iAddrFamily != AF_INET )
    {
        struct addrinfo hints;
        struct addrinfo *aiRes = 0;
        int iError = -1;
        
        memset( &hints, 0x00, sizeof(hints) );
        hints.ai_family = AF_INET6;
        if( 0 == (iError=getaddrinfo(cstrHostName, 0, &hints, &aiRes)) )
        {
            this->SetAddrType( aiRes->ai_family );
            this->SetAddress( aiRes->ai_addr, aiRes->ai_addrlen );
            this->SetPortNumber( iPort, bNeedEncode );
            freeaddrinfo( aiRes );
            return 0;
        }
        if( AF_INET6 == iAddrFamily )
        {
            if( aiRes )
            {
                freeaddrinfo( aiRes );
            }
            errno = iError;
            return -1;
        }
    }
#endif
    
    iAddrFamily = AF_INET;
    this->SetAddrType( iAddrFamily );
    this->m_uInetAddr.m_AddrIN4.sin_family = AF_INET;
    
    struct in_addr inAddrV4 = { 0 };
    if( 1 == inet_aton(cstrHostName, &inAddrV4) )
    {
        return this->SetInetAddr( iPort
                                , bNeedEncode ? ntohl(inAddrV4.s_addr) : inAddrV4.s_addr
                                , bNeedEncode );
    }
    
    return -1;
}

int InetAddress::SetInetAddr( const char *cstrPort, const char *cstrHostName, int iAddrFamily )
{
    if( 0==cstrPort || 0==cstrHostName )
    {
        errno = EINVAL;
        return -1;
    }
    memset( &this->m_uInetAddr, 0x00, sizeof(this->m_uInetAddr) );
    
    if( AF_UNSPEC == iAddrFamily && !IPv6IsEnabled() )
    {
        iAddrFamily = AF_INET;
    }

    int iError = -1;
    struct addrinfo hints = { 0 };
    struct addrinfo *aiRes = 0;
    hints.ai_family = iAddrFamily;

    if( 0 == (iError=getaddrinfo(cstrHostName, cstrPort, &hints, &aiRes)) )
    {
        this->SetAddrType( aiRes->ai_family );
        this->SetAddress( aiRes->ai_addr, aiRes->ai_addrlen );

        freeaddrinfo( aiRes );
        return 0;
    }
    if( aiRes )
    {
        freeaddrinfo( aiRes );
    }
    errno = iError;
    return -1;
}

InetAddress::InetAddress( uint16 iPort, const char *cstrHostName, int iAddrFamily ): BaseAddress( DetermineType(), sizeof(m_uInetAddr) )
{
    memset( &this->m_uInetAddr, 0x00, sizeof(this->m_uInetAddr) );
    
    this->SetInetAddr( iPort, cstrHostName, true, iAddrFamily );
}

InetAddress::InetAddress( const char *cstrPort, const char *cstrHostName, int iAddrFamily ): BaseAddress( DetermineType(), sizeof(m_uInetAddr) )
{
    this->Reset();
    
    this->SetInetAddr( cstrPort, cstrHostName, iAddrFamily );
}
