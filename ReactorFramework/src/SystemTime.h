#ifndef __SYSTEM_TIME_POLICY_H__
#define __SYSTEM_TIME_POLICY_H__

#include "base.h"
#include "GlobalMacros.h"
#include "TimeValue.h"

class SystemTime
{
public:
    SystemTime( const char *cstrPoolName );
    
    ~SystemTime( void );
    
    static int GetLocalSystemTime( time_t &timeout );
    
    static int GetLocalSystemTime( TimeValue &timeout );
};

class SystemTimePolicy
{
public:
    TimeValueT<SystemTimePolicy> operator() () const;
};

#endif
