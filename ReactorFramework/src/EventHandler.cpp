#include "EventHandler.h"
#include "Reactor.h"

EventHandler::EventHandler( Reactor *reactor, int iPriority )
    : m_ReferenceCount( 1 ), m_iPriority( iPriority )
    , m_pReactor( reactor ), m_RefCntPolicy( ReferenceCountingPolicy::DISABLED )
{
}

EventHandler::~EventHandler( void )
{
}

HANDLE EventHandler::GetHandle( void ) const
{
    return INVALID_HANDLE;
}

void EventHandler::SetHandle( HANDLE handle )
{
}

int EventHandler::HandleInput( HANDLE )
{
    return -1;
}
int EventHandler::HandleOutput( HANDLE )
{
    return -1;
}
int EventHandler::HandleException( HANDLE )
{
    return -1;
}
int EventHandler::HandleTimeout( const TimeValue &currTime, const void *pAct )
{
    return -1;
}

int EventHandler::HandleClose( HANDLE, ReactorMask )
{
    return -1;
}

int EventHandler::ResumeHandler( void )
{
    return EventHandler::REACTOR_RESUMES_HANDLER;
}

Reactor* EventHandler::GetReactor( void ) const
{
    return this->m_pReactor;
}
void EventHandler::SetReactor( Reactor *reactor )
{
    this->m_pReactor = reactor;
}

ReactorTimerInterface* EventHandler::GetReactorTimerInterface( void ) const
{
    return this->m_pReactor;
}

EventHandler::Policy::~Policy( void )
{
}

EventHandler::ReferenceCountingPolicy::ReferenceCountingPolicy( Value value )
    :m_Value( value )
{
}

EventHandler::ReferenceCountingPolicy::Value EventHandler::ReferenceCountingPolicy::GetValue( void ) const
{
    return this->m_Value;
}
void EventHandler::ReferenceCountingPolicy::SetValue( EventHandler::ReferenceCountingPolicy::Value value )
{
    this->m_Value = value;
}
EventHandler::ReferenceCountingPolicy& EventHandler::GetRefCntPolicy( void )
{
    return this->m_RefCntPolicy;
}

EventHandler::ReferenceCount EventHandler::AddReference( void )
{
    bool const bRefCntRequired = this->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

    if( bRefCntRequired )
    {
        return ++this->m_ReferenceCount;
    }
    return 1;
}

EventHandler::ReferenceCount EventHandler::RemoveReference( void )
{
    bool bRefCntRequired = this->GetRefCntPolicy().GetValue() == EventHandler::ReferenceCountingPolicy::ENABLED;

    if( bRefCntRequired )
    {
        EventHandler::ReferenceCount result = --this->m_ReferenceCount;

        if( 0 == result )
        {
            delete this;
        }
        return result;
    }

    return 1;
}

