#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include "BaseAddress.h"

class InetAddress: public BaseAddress
{
public:
    InetAddress( void );
    InetAddress( const InetAddress &addr );
    InetAddress( uint16 iPort, const char *cstrHostName, int iAddrFamily = AF_UNSPEC );
    InetAddress( const char *cstrPort, const char *cstrHostName, int iAddrFamily = AF_UNSPEC );
    //InetAddress( struct sockaddr_in *sockAddrIn, int iAddrLen );
    //InetAddress( const char *cstrPort, uint32 iIPAddr, const char *cstrProtocol = "tcp" );
    //explicit InetAddress( const char *cstrAddress, int iAddrFamily = AF_UNSPEC );
    //explicit InetAddress( uint16 iPort, uint32 iIPAddr = INADDR_ANY );

    virtual ~InetAddress( void );
    
    static bool IPv6IsEnabled( void );
    
    void SetPortNumber( uint16 iPortNum, bool bNeedEncode = true );
    
    int SetInetAddr( const InetAddress &addr );
    int SetInetAddr( const char *SAddr, int iAddrLen, bool bNeedEncode = true, int iMap = 0 );
    int SetInetAddr( uint16 iPort, uint32 iSAddr = INADDR_ANY, bool bNeedEncode = true, int iMap = 0 );
    int SetInetAddr( uint16 iPort, const char* cstrHostName, bool bNeedEncode = true, int iAddrFamily = AF_UNSPEC );
    int SetInetAddr( const char *cstrPort, const char *cstrHostName, int iAddrFamily = AF_UNSPEC );

    virtual void*   GetAddress( void ) const;
    virtual void    SetAddress( void *addr, int iLen );
    virtual void    SetAddress( void *addr, int iLen, int iMap );

private:
    int     DetermineType( void ) const;
    void    Reset( void );

    union
    {
        sockaddr_in     m_AddrIN4;
        
        #ifdef HAS_IPV6
        sockaddr_in6    m_AddrIN6;
        #endif

    }m_uInetAddr;
};


inline int InetAddress::DetermineType( void ) const
{
    #ifdef HAS_IPV6
    return AF_INET6;
    #else
    return AF_INET;
    #endif
}

inline void InetAddress::Reset( void )
{
    memset( &this->m_uInetAddr, 0x00, sizeof(this->m_uInetAddr) );
    if( AF_INET == this->GetAddrType() )
    {
        this->m_uInetAddr.m_AddrIN4.sin_family = AF_INET;
    }
    #ifdef HAS_IPV6
    else if( AF_INET6 == this->GetAddrType() )
    {
        this->m_uInetAddr.m_AddrIn6.sin6_family = AF_INET6
    }
    #endif
}

#endif