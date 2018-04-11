#ifndef __DYNAMIC_H__
#define __DYNAMIC_H__

#include "Public.h"

class Dynamic
{
public:
    Dynamic( void );
    ~Dynamic( void );

    void SetFlag( void );
    void ResetFlag( void );

    bool GetIsDynamic( void );

    static Dynamic* GetInstance( void );

private:
    bool    m_bIsDynamic;
};

inline Dynamic::~Dynamic( void )
{
}

inline void Dynamic::SetFlag( void )
{
    this->m_bIsDynamic = true;
}

inline void Dynamic::ReSetFlag( void )
{
    this->m_bIsDynamic = false;
}

inline bool Dynamic::GetIsDynamic( void )
{
    return this->m_bIsDynamic;
}

#endif

