#ifndef __PUB_FUNC_H__
#define __PUB_FUNC_H__

#include "Public.h"
#include "TimeValue.h"

namespace PubFunc
{
    extern int GetMaxHandles( void );
    extern int SetHandleLimit( int iNewLimit = -1, int iIncreaseLimitOnly = 0 );
    extern TimeValue GetTimeOfDay( void );
};

#endif

