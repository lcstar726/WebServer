#ifndef __REQUEST_EVENT_HANDLER_H__
#define __REQUEST_EVENT_HANDLER_H__

#include "Public.h"
#include "EventHandler.h"
#include "Reactor.h"
#include "SockStream.h"

class RequestEventHandler: public EventHandler
{
public:
    enum{ MAX_WAIT_SECOND = 5, MAX_WAIT_SUSECOND = 10000 };    //0.01√Î
    RequestEventHandler( Reactor *reactor, const TimeValue tvMaxWaitTime = TimeValue(MAX_WAIT_SECOND) )
        : EventHandler( reactor ), m_tvTimeOfLastReq(0), m_tvMaxWaitTime( tvMaxWaitTime )
    {
    }
    virtual ~RequestEventHandler( void ) {}

    virtual int Open( void );
    virtual HANDLE GetHandle( void ) const;
    SockStream & GetClientPeer( void );

    virtual int HandleInput( HANDLE handle = INVALID_HANDLE );
    virtual int HandleTimeout( const TimeValue &now, const void *pAct = NULL );
    virtual int HandleClose( HANDLE handle = INVALID_HANDLE, ReactorMask mask = 0 );

protected:
    SockStream      m_ClientPeer;
    TimeValue       m_tvTimeOfLastReq;
    const TimeValue m_tvMaxWaitTime;
};

#endif

