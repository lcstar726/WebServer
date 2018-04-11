#ifndef __SOCK_IO_H__
#define __SOCK_IO_H__

#include "SockBase.h"
#include "TimeValue.h"

class SockIO: public SockBase
{
public:
    SockIO( void );
    ~SockIO( void );
    
    ssize_t Recv( void *recvBuf, size_t recvCount, const TimeValue *timeout = NULL );
    ssize_t Recv( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout = NULL );
    ssize_t Recvv( iovec ioVect[], int iCount, const TimeValue *timeout = NULL );

    ssize_t Send( void *sendBuf, size_t sendLen, const TimeValue *timeout = NULL );
    ssize_t Send( void *sendBuf, size_t sendLen, int iFlags, const TimeValue *timeout = NULL );
    ssize_t Sendv( iovec ioVect[], int iCount, const TimeValue *timeout = NULL );
    
    int HandleReady( const TimeValue *timeout, bool bReadReady, bool bWriteReady ) const;
    void RecordSetNonBlockMode( int &iOldFlags );
    void RestoreBlockMode( int &iOldFlags );
    int EnterRecvTimedWait( const TimeValue *timeout, int &iOldFlags );
    int EnterSendTimedWait( const TimeValue *timeout, int &iOldFlags );
};

inline SockIO::SockIO( void ) {}
inline SockIO::~SockIO( void ) {}

inline void SockIO::RecordSetNonBlockMode( int &iOldFlags )
{
    iOldFlags = this->GetFlags();
    if( BIT_DISABLED(iOldFlags, O_NONBLOCK) )
    {
        this->SetSingleFlags( O_NONBLOCK );
    }
}

inline void SockIO::RestoreBlockMode( int &iOldFlags )
{
    if( BIT_DISABLED(iOldFlags, O_NONBLOCK) )
    {
        this->ClrFlags( O_NONBLOCK );
    }
}

#endif
