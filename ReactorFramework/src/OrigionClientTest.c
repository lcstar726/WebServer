#include "InetAddress.h"

int main( int argc, char* argv[] )
{
    int iRet = -1;
    
    InetAddress svrInetAddr( 23456, "10.4.86.11", AF_INET );
    
    int cltSockFd = socket( PF_INET, SOCK_STREAM, 0 );
    cout<<"cltSockFd is :"<<cltSockFd<<endl;
    
    iRet = connect( cltSockFd, (struct sockaddr*)svrInetAddr.GetAddress(), svrInetAddr.GetAddrSize() );
    if( -1 == iRet )
    {
        cout<<"connect failed errno="<<errno<<endl;
    }
    else
    {
        size_t iBufSize = 1024;
        char *cstrSendBuffer = NULL;
        char cstrRecvBuffer[iBufSize];
        
        memset( cstrRecvBuffer, 0x00, iBufSize );
        
        while( (iRet = getline( &cstrSendBuffer, &iBufSize, stdin )) != -1 )
        {
            if( strncmp(cstrSendBuffer, "quit", 4 ) == 0 )
            {
                break;
            }
            cout<<"cstrSendBuffer="<<cstrSendBuffer<<endl;
            cout<<"strlen(cstrSendBuffer)="<<strlen(cstrSendBuffer)<<endl;

            int iSendLen = send( cltSockFd, cstrSendBuffer, strlen(cstrSendBuffer)-1, 0 );
            cout<<"Send to Server message is:"<<cstrSendBuffer<<"send length="<<iSendLen<<endl;
            
            int iRecvLen = recv( cltSockFd, cstrRecvBuffer, iBufSize-1, 0 );
            cout<<"from Server message is:"<<cstrRecvBuffer<<"iRecvLen="<<iRecvLen<<endl;
        }
        close( cltSockFd );
    }
    
    return 0;
}
