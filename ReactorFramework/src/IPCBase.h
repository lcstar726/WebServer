#ifndef __IPC_BASE_H__
#define __IPC_BASE_H__

#include "Public.h"

class IPCBase
{
public:
    int Enable( int value );
    
    int Disable( int value );
    
    int GetFlags( void ) const;
    int SetSingleFlags( int iSingleFlags );
    int SetAllFlags( int iAllFlags );
    int ClrFlags( int iSingleFlags );
    
    int Control( int iCmd, void *arg ) const;
    
    HANDLE GetHandle( void ) const;
    
    void SetHandle( HANDLE handle );

protected:
    IPCBase( void );
    ~IPCBase( void );

private:
    HANDLE          m_Handle;
    static pid_t    m_Pid;
};

inline IPCBase::~IPCBase( void ) {}

inline HANDLE IPCBase::GetHandle( void ) const
{
    return this->m_Handle;
}

inline void IPCBase::SetHandle( HANDLE handle )
{
    this->m_Handle = handle;
}

inline int IPCBase::Control( int iCmd, void *arg ) const
{
    return ioctl( this->m_Handle, iCmd, arg );
}

#endif
