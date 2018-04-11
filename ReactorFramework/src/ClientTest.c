#include "InetAddress.h"
#include "SockStream.h"
#include "SockConnector.h"

class ClientPeer
{
public:
    SockStream &GetLocalPeer( void );
    ~ClientPeer( void );

    int Open( int argc, char* argv[] );

    int SendToRemote( const char *cstrBuffer, int iSendCount );
    int RecvFromRemote( char *cstrBuffer, int iRecvCount );
private:
    SockStream m_ssLocalPeer;
};

int ClientPeer::Open( int argc, char* argv[] )
{
    int iRet = 0;
    InetAddress svrInetAddr( 23456, "10.4.86.11", AF_INET );
    SockConnector soConn;
    TimeValue tv( 2 );

    if( -1 == (iRet=soConn.Connect(this->m_ssLocalPeer, svrInetAddr, &tv)) )
    {
        cout<<"client connect server failed"<<endl;
        return iRet;
    }

    return iRet;
}

inline SockStream& ClientPeer::GetLocalPeer( void )
{
    return this->m_ssLocalPeer;
}
inline ClientPeer::~ClientPeer( void )
{
    this->m_ssLocalPeer.CloseSocket();
}

int ClientPeer::SendToRemote( const char* cstrBuffer, int iSendCount )
{
    TimeValue tv( 2 );
    //return this->m_ssLocalPeer.SendN( cstrBuffer, iSendCount );
    return this->m_ssLocalPeer.SendN( cstrBuffer, iSendCount, 0 );
    //return this->m_ssLocalPeer.SendN( cstrBuffer, iSendCount, 0, &tv );
}

int ClientPeer::RecvFromRemote( char *cstrBuffer, int iRecvCount )
{
    TimeValue tv( 2 );
    //return this->m_ssLocalPeer.RecvN( cstrBuffer, iRecvCount );
    return this->m_ssLocalPeer.RecvN( cstrBuffer, iRecvCount, 0 );
    //return this->m_ssLocalPeer.RecvN( cstrBuffer, iRecvCount, 0, &tv );
}


int main( int argc, char* argv[] )
{
    int iRet = -1;
    
    ClientPeer cltPeer;

    if( -1 == cltPeer.Open(argc, argv) )
    {
        cout<<"client connect server failed"<<endl;
    }
    else
    {
        size_t iBufSize = 1024;
        char *cstrSendBuffer = NULL;
        char cstrRecvBuffer[iBufSize];
        
        memset( cstrRecvBuffer, 0x00, iBufSize );
        
        //while( (iRet = getline( &cstrSendBuffer, &iBufSize, stdin )) != -1 )
        iRet = getline( &cstrSendBuffer, &iBufSize, stdin );
        {
            if( strncmp(cstrSendBuffer, "quit", 4 ) == 0 )
            {
                //break;
                return 0;
            }
            cout<<"cstrSendBuffer="<<cstrSendBuffer<<endl;
            cout<<"strlen(cstrSendBuffer)="<<strlen(cstrSendBuffer)<<endl;

            int iSendLen = cltPeer.SendToRemote( cstrSendBuffer, strlen(cstrSendBuffer)-1 );
            cout<<"send to remote server: iSendLen="<<iSendLen<<endl;
            //send( cltSockFd, cstrSendBuffer, strlen(cstrSendBuffer)-1, 0 );
            
            
            //int iRecvLen = recv( cltPeer.GetLocalPeer().GetHandle(), cstrRecvBuffer, iBufSize-1, 0 );
            int iRecvLen = cltPeer.RecvFromRemote( cstrRecvBuffer, iBufSize );
            cout<<"iRecvLen="<<iRecvLen<<endl;
        
            cout<<"message of remote server is:"<<cstrRecvBuffer<<endl;
        }
    }
    
    return 0;
}

