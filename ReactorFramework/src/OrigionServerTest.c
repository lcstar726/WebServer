#include "InetAddress.h"

int main( int argc, char* argv[] )
{
    int iRet = -1;

    InetAddress svrInetAddr( 23456, "10.4.86.11", AF_INET );
   
    int svrSockFd = socket( PF_INET, SOCK_STREAM, 0 );
    cout<<"svrSockFd="<<svrSockFd<<endl;
    
    iRet = bind( svrSockFd, (struct sockaddr*)svrInetAddr.GetAddress(), svrInetAddr.GetAddrSize() );
    cout<<"bind ret="<<iRet<<endl;
    
    iRet = listen( svrSockFd, 5 );
    cout<<"listen ret="<<iRet<<endl;
    
    struct sockaddr_in cltAddr;
    socklen_t cltAddrLen = sizeof( cltAddr );

    while( true )
    {
        int acceptFd = accept( svrSockFd, (struct sockaddr*)&cltAddr, &cltAddrLen );
        
        if( acceptFd > 0 )
        {
            int iBufSize = 1024;
            char cstrRecvBuffer[iBufSize];
            
            int iRecvLen = 0;
            int iCurrPos = 0;
            memset( cstrRecvBuffer, 0x00, iBufSize );
            //while( true)
            {
                iRecvLen = recv( acceptFd, &cstrRecvBuffer[iCurrPos], iBufSize-1-iCurrPos, 0 );
                cout<<"iRecvLen="<<iRecvLen<<endl;
                cout<<"iCurrPos="<<iCurrPos<<endl;
                iCurrPos += iRecvLen;
            
                cout<<"from client message is:"<<cstrRecvBuffer<<endl;
                send( acceptFd, cstrRecvBuffer, strlen(cstrRecvBuffer), 0 );

                close( acceptFd );
            }
        }
    }
    close( svrSockFd );
    
    return 0;
}
