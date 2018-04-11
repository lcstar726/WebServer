#ifndef __SOCKBASE_H__
#define __SOCKBASE_H__

#include "Public.h"
#include "BaseAddress.h"
#include "IPCBase.h"

class SockBase: public IPCBase
{
public:
    int SetSockOption( int iLevel, int iOption, void *optVal, int iOptLen ) const;
    int GetSockOption( int iLevel, int iOption, void *optVal, int *iOptLen ) const;
    
    int Open( int iType, int iProtocolFamily, int iProtocol, bool bIsReuseAddr );
    int CloseSocket( void );
    int GetLocalAddress( BaseAddress &addr ) const;
    int GetRemoteAddress( BaseAddress &addr ) const;

protected:
    SockBase( void );
    SockBase( int iType, int iProtocolFamily, int iProtocol = 0, bool bIsReuseAddr = false );
    ~SockBase( void );
};

inline SockBase::~SockBase( void ) {}

inline int SockBase::SetSockOption( int iLevel, int iOption, void *optVal, int iOptLen ) const
{
    return setsockopt( this->GetHandle(), iLevel, iOption, (const char*)optVal, iOptLen );
}

inline int SockBase::GetSockOption( int iLevel, int iOption, void *optVal, int *iOptLen ) const
{
    return getsockopt( this->GetHandle(), iLevel, iOption, (char*)optVal, reinterpret_cast<socklen_t*>(iOptLen) );
}

#endif
