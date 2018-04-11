#include "PubFunc.h"

int PubFunc::GetMaxHandles( void )
{
    struct rlimit rl;
    memset( &rl, 0x00, sizeof(rl) );

    int iRet = getrlimit( RLIMIT_NOFILE, &rl );
    if( 0 == iRet && rl.rlim_cur != RLIM_INFINITY )
    {
        return rl.rlim_cur;
    }
    else
    {
        cout<<"PubFunc::GetMaxHandle failed"<<endl;
        errno = ENOTSUP;
        return -1;
    }
}
int PubFunc::SetHandleLimit( int iNewLimit, int iIncreaseLimitOnly )
{
    int iCurLimit = PubFunc::GetMaxHandles();
    int iMaxLimit = iCurLimit;

    if( -1 == iCurLimit )
    {
        cout<<"PubFunc::SetHandleLimit failed!"<<endl;
        return -1;
    }

    struct rlimit rl;
    memset( &rl, 0x00, sizeof(rl) );
    int iRet = getrlimit( RLIMIT_NOFILE, &rl );
    if( 0 == iRet )
    {
        iMaxLimit = rl.rlim_max;
    }

    if( -1 == iNewLimit )
    {
        iNewLimit = iMaxLimit;
    }
    if( iNewLimit < 0 )
    {
        cout<<"PubFunc::SetHandleLimit failed!"<<endl;
        errno = EINVAL;
        return -1;
    }
    else if( iNewLimit > iCurLimit )
    {
        rl.rlim_cur = iNewLimit;
        return setrlimit( RLIMIT_NOFILE, &rl );
    }

    if( 0 == iIncreaseLimitOnly )
    {
        rl.rlim_cur = iNewLimit;
        return setrlimit( RLIMIT_NOFILE, &rl );
    }

    return 0;
}

TimeValue PubFunc::GetTimeOfDay( void )
{
    timeval tv;
    int result = 0;

    OSCALL (::gettimeofday (&tv, 0), int, -1, result);
    //OSCALL (::gettimeofday (&tv), int, -1, result);

    if( -1 == result )
    {
        return TimeValue( (time_t)-1 );
    }
    else
    {
        return TimeValue (tv);
    }
}


