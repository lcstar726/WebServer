#ifndef __TIME_POLICY_H__
#define __TIME_POLICY_H__

#include "Public.h"
#include "TimeValue.h"
#include "PubFunc.h"

class FPointerTimePolicy
{
public:
    FPointerTimePolicy() : m_pFunc( PubFunc::GetTimeOfDay ) {}

    typedef TimeValue (*FPointer)( void );

    FPointerTimePolicy( FPointer func ) { this->m_pFunc = func; }

    void SetGetTimeOfDay( TimeValue (*GetTimeOfDay)(void) ) { this->m_pFunc = GetTimeOfDay; }

    TimeValueT<FPointerTimePolicy> operator ()() const
    {
        return TimeValueT<FPointerTimePolicy>( (*this->m_pFunc)(), *this );
    }

private:
    FPointer    m_pFunc;
};

typedef FPointerTimePolicy DefaultTimePolicy;
#endif

