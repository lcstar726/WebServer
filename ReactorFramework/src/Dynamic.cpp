#include "Dynamic.h"

Dynamic::Dynamic( void )
    : m_bIsDynamic( false )
{
}

Dynamic* Dynamic::GetInstance( void )
{
    return TssSingleton<Dynamic, SYNCH_NULL_MUTEX>::GetInstance();
}

