#include "IPCBase.h"

pid_t IPCBase::m_Pid = 0;

IPCBase::IPCBase( void )
    :m_Handle( INVALID_HANDLE )
{
}

int IPCBase::GetFlags( void ) const
{
    return fcntl( this->m_Handle, F_GETFL, 0 );
}

int IPCBase::SetSingleFlags( int iSingleFlags )
{
    int iOldFlags = fcntl( this->m_Handle, F_GETFL, 0 );
    int iNewFlags = iOldFlags | iSingleFlags;
    //SET_BITS( iNewFlags, iSingleFlags );
    
    if( -1 == fcntl( this->m_Handle, F_SETFL, iNewFlags ) )
    {
        return -1;
    }
    return iOldFlags;
}
int IPCBase::SetAllFlags( int iAllFlags )
{
    if( -1 == fcntl( this->m_Handle, F_SETFL, iAllFlags) )
    {
        return -1;
    }
    return 0;
}

int IPCBase::ClrFlags( int iSingleFlags )
{
    int iNewFlags = fcntl( this->m_Handle, F_GETFL, 0 );
    CLR_BITS( iNewFlags, iSingleFlags );
    
    if( -1 == fcntl( this->m_Handle, F_SETFL, iNewFlags ) )
    {
        return -1;
    }
    return 0;
}

int IPCBase::Enable( int value )
{
    int iValFd = 0;
    switch( value )
    {
        case FD_CLOEXEC:
            iValFd = fcntl( this->m_Handle, F_GETFD );
            iValFd |= FD_CLOEXEC;
            if( -1 == fcntl( this->m_Handle, F_SETFD, iValFd ) )
            {
                return -1;
            }
            break;
        case O_NONBLOCK:
            if( -1 == this->SetSingleFlags( O_NONBLOCK ) )
            {
                return -1;
            }
            break;
        case SIGIO:
            if( -1 == fcntl( this->m_Handle, F_SETOWN, IPCBase::m_Pid )
                || -1 == this->SetSingleFlags( FASYNC ) )
            {
                return -1;
            }
            break;
        case SIGURG:
            return fcntl( this->m_Handle, F_SETOWN, IPCBase::m_Pid );
            break;
        default:
            return -1;
    }
    return 0;
}

int IPCBase::Disable( int value )
{
    int iValFd = 0;
    switch( value )
    {
        case FD_CLOEXEC:
            iValFd = fcntl( this->m_Handle, F_GETFD );
            iValFd &= ~FD_CLOEXEC;
            if( -1 == fcntl( this->m_Handle, F_SETFD, iValFd ) )
            {
                return -1;
            }
            break;        
        case O_NONBLOCK:
            if( -1 == this->ClrFlags( O_NONBLOCK ) )
            {
                return -1;
            }
            break;        
        case SIGURG:
            return fcntl( this->m_Handle, F_SETOWN, 0 );
            break;
        case SIGIO:
            if( -1 == fcntl( this->m_Handle, F_SETOWN, 0 )
                || -1 == ClrFlags( FASYNC ) )
            {
                return -1;
            }
            break;
    }
    return 0;
}
