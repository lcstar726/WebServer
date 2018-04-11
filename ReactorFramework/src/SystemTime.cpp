#include "SystemTime.h"

SystemTime::SystemTime( const char * )
{
}

SystemTime::~SystemTime( void )
{
}

int SystemTime::GetLocalSystemTime( time_t &timeout )
{
    return 0;
}

int SystemTime::GetLocalSystemTime( TimeValue &timeout )
{
    return 0;
}

TimeValueT<SystemTimePolicy> SystemTimePolicy::operator() () const
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    
    return TimeValueT<SystemTimePolicy>( tv );
}