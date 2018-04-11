#include "BaseAddress.h"

const BaseAddress BaseAddress::addrAny( AF_ANY, -1 );

BaseAddress::BaseAddress( int iType, int iSize ): m_iAddrType( iType ), m_iAddrSize( iSize )
{
}

BaseAddress::~BaseAddress( void )
{
}

void* BaseAddress::GetAddress( void ) const
{
    return 0;
}
void BaseAddress::SetAddress( void *addr, int iLen )
{
}
