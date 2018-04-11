#include "TimeValue.h"

const TimeValue TimeValue::zero;
const TimeValue TimeValue::max_time( NumericLimits<time_t>::Max(), ONE_SECOND_IN_USECS-1 );

TimeValue::TimeValue( void )
{
    this->Set( 0, 0 );
}

TimeValue::TimeValue( time_t sec, suseconds_t usec )
{
    this->Set( sec, usec );
}

TimeValue::TimeValue( const struct timeval &tv )
{
    this->Set( tv );
}

TimeValue::~TimeValue( void )
{
}

TimeValue TimeValue::GetTimeOfDay( void )
{
    timeval tv;
    int iRet = 0;

    struct timespec ts;

    if( -1 != (iRet = clock_gettime( CLOCK_REALTIME, &ts )) )
    {
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec / 1000L;
        return TimeValue( tv );
    }
    else if( -1 != (iRet = gettimeofday( &tv, NULL )) )
    {
        return TimeValue( tv );
    }
    else
    {
        return TimeValue( (time_t)-1 );
    }
}

time_t TimeValue::GetSecond( void ) const
{
    return this->m_Timeval.tv_sec;
}
void TimeValue::SetSecond( time_t sec )
{
    this->m_Timeval.tv_sec = sec;
}

suseconds_t TimeValue::GetSuSecond( void ) const
{
    return this->m_Timeval.tv_usec;
}
void TimeValue::SetSuSecond( suseconds_t usec )
{
    this->m_Timeval.tv_usec = usec;
}

void TimeValue::ToSuSecond( uint64 &usec ) const
{
    usec = static_cast<uint64>( this->m_Timeval.tv_sec ) * 1000000;
    usec += this->m_Timeval.tv_usec;
}

ulong TimeValue::GetMilliSecond( void )  const
{
    time_t milliSec = this->m_Timeval.tv_sec * 1000 + this->m_Timeval.tv_usec / 1000;
    return static_cast<ulong>( milliSec );
}

void TimeValue::Set( time_t sec, suseconds_t usec )
{
    this->m_Timeval.tv_sec = sec;
    this->m_Timeval.tv_usec = usec;

#if __GNUC__ && !(__GNUC__ == 3 && __GNUC_MINOR__ == 4)
    if( (__builtin_constant_p(sec) & __builtin_constant_p(usec)) && (sec >= 0 && usec >= 0 && usec < ONE_SECOND_IN_USECS) )
    {
        return;
    }
#endif
}

void TimeValue::Set( const struct timeval &tv )
{
    this->m_Timeval.tv_sec = tv.tv_sec;
    this->m_Timeval.tv_usec = tv.tv_usec;
}

bool operator == ( const TimeValue &tv1, const TimeValue &tv2 )
{
    if( tv1.GetSecond() == tv2.GetSecond() && tv1.GetSuSecond() == tv2.GetSuSecond() )
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool operator != ( const TimeValue &tv1, const TimeValue &tv2 )
{
    return !( tv1 == tv2 );
}

bool operator >( const TimeValue &tv1, const TimeValue &tv2 )
{
    if( tv1.GetSecond() > tv2.GetSecond() )
    {
        return true;
    }
    else if( tv1.GetSecond() == tv2.GetSecond() && tv1.GetSuSecond() > tv2.GetSuSecond() )
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool operator >=( const TimeValue &tv1, const TimeValue &tv2 )
{
    if( tv1.GetSecond() > tv2.GetSecond() )
    {
        return true;
    }
    else if( tv1.GetSecond() == tv2.GetSecond() && tv1.GetSuSecond() >= tv2.GetSuSecond() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool operator <( const TimeValue &tv1, const TimeValue &tv2 )
{
    return tv2 > tv1;
}
bool operator <=( const TimeValue &tv1, const TimeValue &tv2 )
{
    return tv2 >= tv1;
}

TimeValue& TimeValue::operator +=( const TimeValue &tv )
{
    this->SetSecond( this->GetSecond() + tv.GetSecond() );
    this->SetSuSecond( this->GetSuSecond() + tv.GetSuSecond() );

    return *this;
}
TimeValue& TimeValue::operator +=( time_t sec )
{
    this->SetSecond( this->GetSecond() + sec );

    return *this;
}
TimeValue& TimeValue::operator -=( const TimeValue &tv )
{
    this->SetSecond( this->GetSecond() - tv.GetSecond() );
    this->SetSuSecond( this->GetSuSecond() - tv.GetSuSecond() );

    return *this;
}
TimeValue& TimeValue::operator -=( time_t sec )
{
    this->SetSecond( this->GetSecond() - sec );

    return *this;
}
TimeValue& TimeValue::operator *=( double dMultiple )
{
    typedef IfThenElse< (sizeof(double) > sizeof(time_t)), double, long double>::Result timeType;

    timeType secTotal = this->GetSecond() * dMultiple;
    time_t secInteger = static_cast<time_t>( secTotal );
    timeType secDecimal = secTotal - secInteger;

    timeType susecTotal = this->GetSuSecond() * dMultiple + secDecimal * ONE_SECOND_IN_USECS;
    secInteger += susecTotal / ONE_SECOND_IN_USECS;

    suseconds_t susecInteger = static_cast<suseconds_t>(susecTotal) % ONE_SECOND_IN_USECS;

    this->Set( secInteger, susecInteger );

    return *this;
}

TimeValue operator +( const TimeValue &tv1, const TimeValue &tv2 )
{
    TimeValue tvRes( tv1 );
    tvRes += tv2;

    return tvRes;
}

TimeValue operator -( const TimeValue &tv1, const TimeValue &tv2 )
{
    TimeValue tvRes( tv1 );
    tvRes -= tv2;

    return tvRes;
}

TimeValue operator *( double dMultiple, const TimeValue &tv)
{
    return TimeValue(tv) *= dMultiple;
}
TimeValue operator *( const TimeValue &tv, double dMultiple )
{
    return TimeValue(tv) *= dMultiple;
}

TimeValue::operator timespec_t() const
{
    timespec_t ts;
    ts.tv_sec = this->GetSecond();
    ts.tv_nsec = this->GetMilliSecond() * 1000;

    return ts;
}

TimeValue::operator timeval() const
{
    return this->m_Timeval;
}

TimeValue::operator const timeval*() const
{
    return (const timeval*)( &this->m_Timeval );
}

