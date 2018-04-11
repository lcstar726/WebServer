#ifndef __SOCK_STREAM_H__
#define __SOCK_STREAM_H__

#include "SockIO.h"

class SockStream: public SockIO
{
public:
    SockStream( void );
    SockStream( HANDLE handle );
    ~SockStream( void );
    
    ssize_t RecvN( void *recvBuf, size_t recvCount, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t RecvNAux( void *recvBuf, size_t recvCount, size_t *bytesTransferred );
    ssize_t RecvNAux( void *recvBuf, size_t recvCount, const TimeValue *timeout, size_t *bytesTransferred );
    
    ssize_t RecvN( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t RecvNAux( void *recvBuf, size_t recvCount, int iFlags, size_t *bytesTransferred );
    ssize_t RecvNAux( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred );
    
    ssize_t RecvvN( iovec ioVect[], int iCount, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t RecvvNAux( iovec ioVect[], int iCount, size_t *bytesTransferred );
    ssize_t RecvvNAux( iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred );
    
    ssize_t SendN( const void *sendBuf, size_t sendCount, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t SendNAux( const void *sendBuf, size_t sendCount, size_t *bytesTransferred );
    ssize_t SendNAux( const void *sendBuf, size_t sendCount, const TimeValue *timeout, size_t *bytesTransferred );
    
    ssize_t SendN( const void *sendBuf, size_t sendCount, int iFlags, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t SendNAux( const void *sendBuf, size_t sendCount, int iFlags, size_t *bytesTransferred );
    ssize_t SendNAux( const void *sendBuf, size_t sendCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred );
    
    ssize_t SendvN( const iovec ioVect[], int iCount, const TimeValue *timeout = NULL, size_t *bytesTransferred = NULL );
    ssize_t SendvNAux( const iovec ioVect[], int iCount, size_t *bytesTransferred );
    ssize_t SendvNAux( const iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred );
    
    int CloseReader( void );
    int CloseWriter( void );
    int CloseSocket( void );
};

inline SockStream::SockStream( void ) {}
inline SockStream::SockStream( HANDLE handle )
{
    this->SetHandle( handle );
}
inline SockStream::~SockStream( void ) {}
    
inline int SockStream::CloseReader( void )
{
    if( INVALID_HANDLE != this->GetHandle() )
    {
        return shutdown( this->GetHandle(), SHUT_RD );
    }
    return 0;
}
inline int SockStream::CloseWriter( void )
{
    if( INVALID_HANDLE != this->GetHandle() )
    {
        return shutdown( this->GetHandle(), SHUT_WR );
    }
    return 0;
}

#endif
