#ifndef __COPY_DISABLED_H__
#define __COPY_DISABLED_H__

#include "Public.h"

class CopyDisabled
{
public:
    CopyDisabled( void ) {}

private:
    CopyDisabled( const CopyDisabled& );
    CopyDisabled& operator=( const CopyDisabled& );
};

#endif

