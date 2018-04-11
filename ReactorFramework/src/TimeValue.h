#ifndef __TIME_VALUE_H__
#define __TIME_VALUE_H__

#include "Public.h"

#define ONE_SECOND_IN_MSECS 1000L
suseconds_t const ONE_SECOND_IN_USECS = 1000000;
#define ONE_SECOND_IN_NSECS 1000000000L

typedef struct timespec timespec_t;

class TimeValue
{
public:
    TimeValue( void );
    
    explicit TimeValue( time_t sec, suseconds_t usec = 0 );
    
    explicit TimeValue( const struct timeval &tv );
    
    virtual ~TimeValue( void );

    time_t GetSecond( void ) const;
    void SetSecond( time_t sec );
    suseconds_t GetSuSecond( void ) const;
    void SetSuSecond( suseconds_t usec );
    ulong GetMilliSecond( void ) const;
    void ToSuSecond( uint64 &usec ) const;
    void Set( time_t sec, suseconds_t usec );
    void Set( const struct timeval &tv );

    TimeValue& operator +=( const TimeValue &tv );
    TimeValue& operator +=( time_t sec );
    TimeValue& operator -=( const TimeValue &tv );
    TimeValue& operator -=( time_t sec );
    TimeValue& operator *=( double dMultiple );
    
    friend bool operator == ( const TimeValue &tv1, const TimeValue &tv2 );
    friend bool operator != ( const TimeValue &tv1, const TimeValue &tv2 );
    friend bool operator >( const TimeValue &tv1, const TimeValue &tv2 );
    friend bool operator >=( const TimeValue &tv1, const TimeValue &tv2 );
    friend bool operator <( const TimeValue &tv1, const TimeValue &tv2 );
    friend bool operator <=( const TimeValue &tv1, const TimeValue &tv2 );
    friend TimeValue operator +( const TimeValue &tv1, const TimeValue &tv2 );
    friend TimeValue operator -( const TimeValue &tv1, const TimeValue &tv2 );
    friend TimeValue operator *( double dMultiple, const TimeValue &tv );
    friend TimeValue operator *( const TimeValue &tv, double dMultiple );
    
    operator timespec_t() const;
    operator timeval() const;
    operator const timeval*() const;

    TimeValue GetTimeOfDay( void );

public:
    static const TimeValue zero;
    static const TimeValue max_time;
private:
    struct timeval m_Timeval;
};

template<class TIME_POLICY>
class TimeValueT: public TimeValue
{
public:
    typedef TIME_POLICY time_policy_t;
    
    TimeValueT( void ) { }
    
    explicit TimeValueT( time_t sec, suseconds_t usec = 0 ): TimeValue( sec, usec ) { }
        
    explicit TimeValueT( const struct timeval &tv ): TimeValue( tv ) { }

    explicit TimeValueT( const TimeValue &tv, const TIME_POLICY &tp = TIME_POLICY() )
        : TimeValue( tv ), m_TimePolicy( tp )
    {
    }

    ~TimeValueT( void ) { }
    
private:
    time_policy_t m_TimePolicy;
};
#endif

