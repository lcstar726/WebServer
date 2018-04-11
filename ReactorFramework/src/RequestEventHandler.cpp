#include "RequestEventHandler.h"
#include "InetAddress.h"

int RequestEventHandler::Open( void )
{
    cout<<__FILE__<<"_"<<__LINE__<<__FUNCTION__<<"__RequestEventHandler::Open RegisterHandler( this, EventHandler::READ_MASK ) begin"<<endl;

    int iRet = this->GetReactor()->RegisterHandler( this, EventHandler::READ_MASK );

    if( iRet != -1 )
    {
        TimeValue tvReSchedule( this->m_tvMaxWaitTime.GetSecond() / 5 );

        cout<<__FILE__<<"_"<<__LINE__<<__FUNCTION__<<"__RequestEventHandler::Open this->GetReactor()->ScheduleTimer() begin"<<endl;
        iRet = this->GetReactor()->ScheduleTimer( this, NULL, this->m_tvMaxWaitTime, tvReSchedule );
    }

    return iRet;
}

HANDLE RequestEventHandler::GetHandle( void ) const
{
    return m_ClientPeer.GetHandle();
}

SockStream& RequestEventHandler::GetClientPeer( void )
{
    return this->m_ClientPeer;
}

int RequestEventHandler::HandleInput( HANDLE handle )
{
    cout<<__FILE__<<"_"<<__LINE__<<__FUNCTION__<<"__RequestEventHandler::HandleInput handle="<<handle<<endl;

    this->m_tvTimeOfLastReq = this->GetReactor()->GetTimerQueue()->GetTimeOfDay();

    size_t iBufSize = 1024;
    char cstrRecvBuffer[iBufSize];
    memset( cstrRecvBuffer, 0x00, sizeof(cstrRecvBuffer) );

    TimeValue tv( 2 );
    //int iRecvLen = this->m_ClientPeer.RecvN( cstrRecvBuffer, iBufSize, 0, &tv );
    int iRecvLen = this->m_ClientPeer.RecvN( cstrRecvBuffer, iBufSize, 0 );
    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleInput receive from remote client: iRecvLen="<<iRecvLen<<"__cstrRecvBuffer="<<cstrRecvBuffer<<endl;

    if( 0 == iRecvLen )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleTimeout this->GetReactor()->RemoveHandler(this, EventHandler::READ_MASK ) Begin!"<<endl;
        this->GetReactor()->RemoveHandler( this, EventHandler::READ_MASK );

        return 0;
    }
    
    int iSendLen = this->m_ClientPeer.SendN( cstrRecvBuffer, strlen(cstrRecvBuffer), 0, &tv );
    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleInput send to remote client: iSendLen="<<iSendLen<<endl;
    
    return 0;
}

int RequestEventHandler::HandleTimeout( const TimeValue &now, const void *pAct )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleTimeout this->m_tvTimeOfLastReq.GetSecond()="<<this->m_tvTimeOfLastReq.GetSecond()<<endl;
    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleTimeout now.GetSecond()="<<now.GetSecond()<<endl;
    if( now - this->m_tvTimeOfLastReq >= this->m_tvMaxWaitTime )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleTimeout this->GetReactor()->RemoveHandler(this, EventHandler::READ_MASK ) Begin!"<<endl;
        this->GetReactor()->RemoveHandler( this, EventHandler::READ_MASK );
    }

    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleTimeout() Finish!"<<endl;
    return 0;
}


int RequestEventHandler::HandleClose( HANDLE handle, ReactorMask mask )
{
    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleClose( handle="<<handle<<", mask="<<mask<<" ) Begin!"<<endl;

    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleClose this->GetReactor()->CancelTimer( this ) Begin!"<<endl;
    this->GetReactor()->CancelTimer( this );

    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleClose this->m_ClientPeer.CloseSocket()!"<<endl;
    this->m_ClientPeer.CloseSocket();

    cout<<__FILE__<<"_"<<__LINE__<<"__RequestEventHandler::HandleClose delete this"<<endl;
    delete this;

    return 0;
}

