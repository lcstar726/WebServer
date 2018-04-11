#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include "Public.h"

#define AF_ANY          -1

class BaseAddress
{
public:
    BaseAddress( int iType = AF_ANY, int iSize = -1 );
    virtual ~BaseAddress( void );
    
    int     GetAddrType( void ) const;
    void    SetAddrType( int iType );
    
    int     GetAddrSize( void ) const;
    void    SetAddrSize( int iSize );
    
    void    BaseSet( int iType, int iSize );
    
    virtual void*   GetAddress( void ) const;
    virtual void    SetAddress( void *addr, int iLen );
    
    bool operator == ( const BaseAddress &addr ) const;
    bool operator != ( const BaseAddress &addr ) const;

    static const BaseAddress addrAny;
protected:
    int m_iAddrType;
    int m_iAddrSize;
};

inline int BaseAddress::GetAddrType( void ) const
{
    return this->m_iAddrType;
}
inline void BaseAddress::SetAddrType( int iType )
{
    this->m_iAddrType = iType;
}

inline int BaseAddress::GetAddrSize( void ) const
{
    return this->m_iAddrSize;
}
inline void BaseAddress::SetAddrSize( int iSize )
{
    this->m_iAddrSize = iSize;
}

inline void BaseAddress::BaseSet( int iType, int iSize )
{
    this->m_iAddrType = iType;
    this->m_iAddrSize = iSize;
}

inline bool BaseAddress::operator == ( const BaseAddress &addr ) const
{
    return ( this->m_iAddrType == addr.m_iAddrType && this->m_iAddrSize == addr.m_iAddrType );
}
inline bool BaseAddress::operator != ( const BaseAddress &addr ) const
{
    return ( this->m_iAddrType != addr.m_iAddrType || this->m_iAddrSize != addr.m_iAddrType );
}

#endif