#include "SockIO.h"

int SockIO::HandleReady( const TimeValue *timeout, bool bReadReady, bool bWriteReady ) const
{
    struct pollfd pollFd;
    
    pollFd.fd = this->GetHandle();
    
    pollFd.events = bReadReady ? POLLIN : 0;
    if( bWriteReady )
    {
        pollFd.events |= POLLOUT;
    }
    pollFd.revents = 0;

    cout<<"SockIO::HandleReady call_poll begin()"<<endl;
    int const iRet = poll( &pollFd, 1, (int)timeout->GetMilliSecond() );
    cout<<"SockIO::HandleReady call_poll iRet="<<iRet<<endl;

    switch( iRet )
    {
        case 0:
            errno = ETIME;
            cout<<"SockIO::HandleReady call_poll timeout"<<endl;
            return -1;
        case -1:
            cout<<"SockIO::HandleReady call_poll errno="<<errno<<endl;
            return -1;
        case 1:
        default:
            return iRet;
    }
}
    
int SockIO::EnterRecvTimedWait( const TimeValue *timeout, int &iOldFlags )
{
    int const iRet = HandleReady( timeout, true, false );
    
    this->RecordSetNonBlockMode( iOldFlags );
    
    return iRet;
}
    
int SockIO::EnterSendTimedWait( const TimeValue *timeout, int &iOldFlags )
{
    int const iRet = HandleReady( timeout, false, true );
    
    this->RecordSetNonBlockMode( iOldFlags );
    
    return iRet;
}

ssize_t SockIO::Recv( void *recvBuf, size_t recvCount, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return read( this->GetHandle(), recvBuf, recvCount );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterRecvTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            ssize_t bytesTransferred = read( this->GetHandle(), recvBuf, recvCount );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}

ssize_t SockIO::Recv( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return recv( this->GetHandle(), recvBuf, recvCount, iFlags );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterRecvTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            ssize_t bytesTransferred = recv( this->GetHandle(), recvBuf, recvCount, iFlags );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}

ssize_t SockIO::Recvv( iovec ioVect[], int iCount, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return readv( this->GetHandle(), ioVect, iCount );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterRecvTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            ssize_t bytesTransferred = readv( this->GetHandle(), ioVect, iCount );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}

ssize_t SockIO::Send( void *sendBuf, size_t sendLen, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return write( this->GetHandle(), sendBuf, sendLen );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterSendTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            ssize_t bytesTransferred = write( this->GetHandle(), sendBuf, sendLen );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}

ssize_t SockIO::Send( void *sendBuf, size_t sendLen, int iFlags, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return send( this->GetHandle(), sendBuf, sendLen, iFlags );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterSendTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            int bytesTransferred = send( this->GetHandle(), sendBuf, sendLen, iFlags );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}

ssize_t SockIO::Sendv( iovec ioVect[], int iCount, const TimeValue *timeout )
{
    if( NULL == timeout )
    {
        return writev( this->GetHandle(), ioVect, iCount );
    }
    else
    {
        int iOldFlags = 0;
        if( -1 == this->EnterSendTimedWait(timeout, iOldFlags) )
        {
            return -1;
        }
        else
        {
            int bytesTransferred = writev( this->GetHandle(), ioVect, iCount );
            this->SetAllFlags( iOldFlags );
            return bytesTransferred;
        }
    }
}
