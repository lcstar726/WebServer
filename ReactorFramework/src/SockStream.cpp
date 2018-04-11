#include "SockStream.h"

int SockStream::CloseSocket( void )
{
    return SockBase::CloseSocket();
}

ssize_t SockStream::RecvNAux( void *recvBuf, size_t recvCount, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t &recvLen = ( NULL==bytesTransferred ? recvTempLen : *bytesTransferred );
    
    ssize_t currLen = 0;
    for( recvLen=0; recvLen<recvCount; recvLen+=currLen )
    {
        currLen = read( this->GetHandle(), static_cast<char*>(recvBuf)+recvLen, recvCount-recvLen );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAux call_read currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( 0, true, false );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //return currLen;
            return static_cast<ssize_t>( recvLen );
        }
    }
    
    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvNAux( void *recvBuf, size_t recvCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t &recvLen = ( NULL == bytesTransferred ? recvTempLen : *bytesTransferred );
    
    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    ssize_t result = -1;
    bool bError = false;
    ssize_t currLen = 0;
    for( recvLen = 0; recvLen < recvCount; recvLen += currLen )
    {
        currLen = read( this->GetHandle(), static_cast<char*>(recvBuf) + recvLen, recvCount - recvLen );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAuxTO call_read currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;

        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( timeout, true, false );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAuxTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //bError = true;
            //result = currLen;
            break;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }
    
    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvN( void *recvBuf, size_t recvCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return RecvNAux( recvBuf, recvCount, bytesTransferred );
    }
    else
    {
        return RecvNAux( recvBuf, recvCount, timeout, bytesTransferred );
    }
}

ssize_t SockStream::RecvNAux( void *recvBuf, size_t recvCount, int iFlags, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t &recvLen = ( NULL==bytesTransferred ? recvTempLen : *bytesTransferred );
    
    char cstrRecvBuffer[recvCount];
    memset( cstrRecvBuffer, 0x00, sizeof(cstrRecvBuffer) );
    ssize_t currLen = 0;
    for( recvLen=0; recvLen<recvCount; recvLen+=currLen )
    {
        currLen = recv( this->GetHandle(), static_cast<char*>(recvBuf)+recvLen, recvCount-recvLen, iFlags );
        //currLen = recv( this->GetHandle(), cstrRecvBuffer+recvLen, recvCount-recvLen, iFlags );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAuxF call_recv curr_recvBuf="<<static_cast<char*>(recvBuf)<<"__currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<"__EINPROGRESS="<<EINPROGRESS<<endl;

        if( EINPROGRESS == errno || 0 == errno )
        {
            return static_cast<ssize_t>( currLen );
        }
        else if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( 0, true, false );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //return currLen;
            break;
        }
    }
    
    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvNAux( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t &recvLen = ( NULL == bytesTransferred ? recvTempLen : *bytesTransferred );
    
    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    ssize_t result = -1;
    bool bError = false;
    ssize_t currLen = 0;
    for( recvLen = 0; recvLen < recvCount; recvLen += currLen )
    {
        currLen = recv( this->GetHandle()
                        , static_cast<char*>(recvBuf) + recvLen
                        , recvCount - recvLen
                        , iFlags );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAuxFTO call_recv currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;

        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( timeout, true, false );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAuxFTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            bError = true;  //socket was closed or received EINTR signal
            result = currLen + recvLen;
            break;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }
    
    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvN( void *recvBuf, size_t recvCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return RecvNAux( recvBuf, recvCount, iFlags, bytesTransferred );
    }
    else
    {
        return RecvNAux( recvBuf, recvCount, iFlags, timeout, bytesTransferred );
    }
}

ssize_t SockStream::RecvvNAux( iovec ioVect[], int iCount, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t recvLen = ( NULL == bytesTransferred ? recvTempLen : *bytesTransferred );

    recvLen = 0;
    
    for( int iCurrIdx = 0; iCurrIdx < iCount; )
    {
        ssize_t currLen = readv( this->GetHandle(), ioVect+iCurrIdx, iCount-iCurrIdx );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvNAux call_readv currLen="<<currLen<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( 0, true, false );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            return currLen;
        }
        
        for( recvLen += currLen; iCurrIdx < iCount && currLen >= static_cast<ssize_t>(ioVect[iCurrIdx].iov_len); ++iCurrIdx )
        {
            currLen -= ioVect[iCurrIdx].iov_len;
        }
        if( currLen > 0 )
        {
            char *base = static_cast<char*>( ioVect[iCurrIdx].iov_base );
            ioVect[iCurrIdx].iov_base = base + currLen;
            ioVect[iCurrIdx].iov_len -= currLen;
        }
    }
    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvvNAux( iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t recvTempLen = 0;
    size_t recvLen = ( NULL == bytesTransferred ? recvTempLen : *bytesTransferred );
    recvLen = 0;
    
    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    ssize_t result = -1;
    bool bError = false;
    for( int iCurrIdx = 0; iCurrIdx < iCount; )
    {
        ssize_t currLen = readv( this->GetHandle(), ioVect+iCurrIdx, iCount-iCurrIdx );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvvNAuxTO call_recv currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && EWOULDBLOCK == errno )
            {
                int const iRet = this->HandleReady( timeout, true, false );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::RecvvNAuxTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //bError = true;
            //result = currLen;
            break;
        }
        
        for( recvLen += currLen; iCurrIdx < iCount && currLen >= static_cast<ssize_t>(ioVect[iCurrIdx].iov_len); ++iCurrIdx )
        {
            currLen -= ioVect[iCurrIdx].iov_len;
        }
        if( currLen > 0 )
        {
            char *base = static_cast<char*>( ioVect[iCurrIdx].iov_base );
            ioVect[iCurrIdx].iov_base = base + currLen;
            ioVect[iCurrIdx].iov_len -= currLen;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }

    return static_cast<ssize_t>( recvLen );
}

ssize_t SockStream::RecvvN( iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return RecvvNAux( ioVect, iCount, bytesTransferred );
    }
    else
    {
        return RecvvNAux( ioVect, iCount, timeout, bytesTransferred );
    }
}

ssize_t SockStream::SendNAux( const void *sendBuf, size_t sendCount, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );

    ssize_t currLen = 0;
    for( sendLen = 0; sendLen < sendCount; sendLen += currLen )
    {
        currLen = write( this->GetHandle(), static_cast<const char*>(sendBuf)+sendLen, sendCount );
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = HandleReady( 0, true, false );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            return currLen;
        }
    }
    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendNAux( const void *sendBuf, size_t sendCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );

    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    bool bError = false;
    ssize_t result = -1;
    ssize_t currLen = 0;
    for( sendLen = 0; sendLen < sendCount; sendLen += currLen )
    {
        currLen = write( this->GetHandle(), static_cast<const char*>(sendBuf)+sendLen, sendCount );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendNAuxTO call_write currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = HandleReady( timeout, true, false );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendNAuxTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //bError = true;
            //result = currLen;
            break;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }

    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendN( const void *sendBuf, size_t sendCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return SendNAux( sendBuf, sendCount, bytesTransferred );
    }
    else
    {
        return SendNAux( sendBuf, sendCount, timeout, bytesTransferred );
    }
}

ssize_t SockStream::SendNAux( const void *sendBuf, size_t sendCount, int iFlags, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );

    ssize_t currLen = 0;
    for( sendLen = 0; sendLen < sendCount; sendLen += currLen )
    {
        currLen = send( this->GetHandle(), static_cast<const char*>(sendBuf)+sendLen, sendCount, iFlags );
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = HandleReady( 0, true, false );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            return currLen;
        }
    }
    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendNAux( const void *sendBuf, size_t sendCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );

    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    bool bError = false;
    ssize_t result = -1;
    ssize_t currLen = 0;
    for( sendLen = 0; sendLen < sendCount; sendLen += currLen )
    {
        currLen = send( this->GetHandle(), static_cast<const char*>(sendBuf)+sendLen, sendCount, iFlags );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendNAuxFTO call_send currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = HandleReady( timeout, true, false );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendNAuxFTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //bError = true;
            //result = currLen;
            break;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }

    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendN( const void *sendBuf, size_t sendCount, int iFlags, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return SendNAux( sendBuf, sendCount, iFlags, bytesTransferred );
    }
    else
    {
        return SendNAux( sendBuf, sendCount, iFlags, timeout, bytesTransferred );
    }
}

ssize_t SockStream::SendvNAux( const iovec ioVect[], int iCount, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );
    sendLen = 0;
    
    iovec *iov = const_cast<iovec*>( ioVect );
    
    for( int iCurrIdx = 0; iCurrIdx < iCount; )
    {
        ssize_t currLen = writev( this->GetHandle(), iov + iCurrIdx, iCount - iCurrIdx );
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = this->HandleReady( 0, false, true );
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
                return currLen;
            }
        }
        for( sendLen += currLen; iCurrIdx < iCount && currLen >= static_cast<ssize_t>(iov[iCurrIdx].iov_len); ++iCurrIdx )
        {
            currLen -= iov[iCurrIdx].iov_len;
        }
        if( sendLen > 0 )
        {
            char *base = reinterpret_cast<char*>(iov[iCurrIdx].iov_base);
            iov[iCurrIdx].iov_base = base + currLen;
            iov[iCurrIdx].iov_len -= currLen;
        }
    }
    
    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendvNAux( const iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    size_t sendTempLen = 0;
    size_t &sendLen = ( NULL == bytesTransferred ? sendTempLen : *bytesTransferred );
    sendLen = 0;
    
    int iOldFlags = 0;
    this->RecordSetNonBlockMode( iOldFlags );

    bool bError = false;
    ssize_t result = -1;
    iovec *iov = const_cast<iovec*>( ioVect );

    for( int iCurrIdx = 0; iCurrIdx < iCount; )
    {
        ssize_t currLen = writev( this->GetHandle(), iov + iCurrIdx, iCount - iCurrIdx );
        cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendvNAuxTO call_write currLen="<<currLen<<"__errno="<<errno<<"__EWOULDBLOCK="<<EWOULDBLOCK<<endl;
        
        if( 0 == currLen || -1 == currLen )
        {
            if( -1 == currLen && ( EWOULDBLOCK == errno || ENOBUFS == errno ) )
            {
                int const iRet = this->HandleReady( timeout, false, true );
                cout<<__FILE__<<"_"<<__LINE__<<"__SockStream::SendvNAuxTO call_HandleReady iRet="<<iRet<<endl;
                if( -1 != iRet )
                {
                    currLen = 0;
                    continue;
                }
            }
            //bError = true;
            //result = currLen;
            break;
        }
        for( sendLen += currLen; iCurrIdx < iCount && currLen >= static_cast<ssize_t>(iov[iCurrIdx].iov_len); ++iCurrIdx )
        {
            currLen -= iov[iCurrIdx].iov_len;
        }
        if( sendLen > 0 )
        {
            char *base = reinterpret_cast<char*>(iov[iCurrIdx].iov_base);
            iov[iCurrIdx].iov_base = base + currLen;
            iov[iCurrIdx].iov_len -= currLen;
        }
    }
    this->SetAllFlags( iOldFlags );
    
    if( bError )
    {
        return result;
    }

    return static_cast<ssize_t>( sendLen );
}

ssize_t SockStream::SendvN( const iovec ioVect[], int iCount, const TimeValue *timeout, size_t *bytesTransferred )
{
    if( NULL == timeout )
    {
        return SendvNAux( ioVect, iCount, bytesTransferred );
    }
    else
    {
        return SendvNAux( ioVect, iCount, timeout, bytesTransferred );
    }
}

