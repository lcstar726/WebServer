#include "SockAcceptor.h"
#include "InetAddress.h"

class ServerTest
{
public:
    int ServiceProcess();
    int Open();

private:
    InetAddress     m_AddrLocal;
    SockAcceptor    m_soAcceptor;
    SockStream      m_ssRemotePeer;
};

int ServerTest::Open()
{
    if( -1 == this->m_AddrLocal.SetInetAddr( 23456, "10.4.86.11", AF_INET ) )
    {
        cout<<"ServerTest::ServiceProcess m_AddrLocal.SetInetAddr failed!"<<endl;
        return -1;
    }

    if( -1 == this->m_soAcceptor.Open( this->m_AddrLocal, 0 ) )
    {
        cout<<"ServerTest::ServiceProcess m_soAcceptor.Open failed!"<<endl;
        return -1;
    }
    return 0;
}

int ServerTest::ServiceProcess()
{
    if( -1 == this->Open() )
    {
        cout<<"ServerTest::ServiceProcess this->Open failed!"<<endl;
        return -1;
    }
    for( ;; )
    {
        if( -1 == this->m_soAcceptor.Accept( this->m_ssRemotePeer ) )
        {
            cout<<"ServerTest::ServiceProcess m_soAcceptor.Accept failed!"<<endl;
            return -1;
        }

        size_t iBufSize = 1024;
        char cstrRecvBuffer[iBufSize];
        memset( cstrRecvBuffer, 0x00, sizeof(cstrRecvBuffer) );

        TimeValue tv( 2 );
        //int iRecvLen = this->m_ssRemotePeer.RecvN( cstrRecvBuffer, iBufSize, 0, &tv );
        int iRecvLen = this->m_ssRemotePeer.RecvN( cstrRecvBuffer, iBufSize, 0 );
        cout<<"receive from remote client: iRecvLen="<<iRecvLen<<"__cstrRecvBuffer="<<cstrRecvBuffer<<endl;

        //int iSendLen = this->m_ssRemotePeer.SendN( cstrRecvBuffer, strlen(cstrRecvBuffer), 0, &tv );
        //cout<<"send to remote client: iSendLen="<<iSendLen<<endl;

        m_ssRemotePeer.CloseSocket();
    }
    return -1 == this->m_soAcceptor.Close() ? -1 : 0;
}

int main( int argc, char* argv[] )
{
    int iRet = -1;

    ServerTest svrTest;

    iRet = svrTest.ServiceProcess();
    
    return iRet;
}

