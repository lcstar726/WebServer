#include "Cleanup.h"

void Cleanup::CleanUp( void * )
{
    delete this;
}
