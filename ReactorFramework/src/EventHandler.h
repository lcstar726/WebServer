#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#include "Public.h"
#include "SynchTraits.h"
#include "AtomicOp.h"
#include "TimeValue.h"

class Reactor;
class ReactorTimerInterface;

typedef unsigned long ReactorMask;
class EventHandler
{
public:
    enum
    {
        LO_PRIORITY = 0
        , HI_PRIORITY = 10
        , NULL_MASK = 0
        , READ_MASK = POLLIN
        , WRITE_MASK = POLLOUT
        , EXCEPT_MASK = POLLPRI

        , ACCEPT_MASK = (1 << 3)
        , CONNECT_MASK = (1 << 4)
        , TIMER_MASK = (1 << 5)
        , QOS_MASK = (1 << 6)
        , GROUP_QOS_MASK = (1 << 7)
        , SIGNAL_MASK = (1 << 8)
        , ALL_EVENTS_MASK = READ_MASK
                            | WRITE_MASK 
                            | EXCEPT_MASK 
                            | ACCEPT_MASK 
                            | CONNECT_MASK 
                            | TIMER_MASK 
                            | QOS_MASK 
                            | GROUP_QOS_MASK 
                            | SIGNAL_MASK
        , RWE_MASK = READ_MASK
                    | WRITE_MASK
                    | EXCEPT_MASK
        , DONT_CALL = (1 << 9)
    };
    enum
    {
        /// The handler is not resumed at all. Could lead to deadlock..
        EVENT_HANDLER_NOT_RESUMED = -1,
        /// The reactor takes responsibility of resuming the handler and
        /// is the default
        REACTOR_RESUMES_HANDLER = 0,
        /// The application takes responsibility of resuming the handler
        APPLICATION_RESUMES_HANDLER
    };

    typedef long ReferenceCount;

    virtual ~EventHandler( void );

    virtual HANDLE GetHandle( void ) const;
    virtual void SetHandle( HANDLE handle );

    virtual int HandleInput( HANDLE fd = INVALID_HANDLE );
    virtual int HandleOutput( HANDLE fd = INVALID_HANDLE );
    virtual int HandleException( HANDLE fd = INVALID_HANDLE );
    virtual int HandleTimeout( const TimeValue &currTime, const void *pAct = NULL );
    virtual int HandleClose( HANDLE handle, ReactorMask closeMask );

    virtual int ResumeHandler( void );

    virtual Reactor* GetReactor( void ) const;
    virtual void SetReactor( Reactor *reactor );

    virtual ReactorTimerInterface* GetReactorTimerInterface( void ) const;

    class Policy
    {
    public:
        virtual ~Policy( void );
    };
    class ReferenceCountingPolicy: public Policy
    {
        friend class EventHandler;
    public:
        enum Value
        {
            ENABLED
            , DISABLED
        };
        Value GetValue( void ) const;
        void SetValue( Value value );

    private:
        ReferenceCountingPolicy( Value value );
        Value m_Value;
    };
    ReferenceCountingPolicy& GetRefCntPolicy( void );

    virtual ReferenceCount AddReference( void );
    virtual ReferenceCount RemoveReference( void );

protected:
    EventHandler( Reactor *reactor = NULL, int iPriority = EventHandler::LO_PRIORITY );

    typedef AtomicOp<SYNCH_MUTEX, ReferenceCount> AtomicReferenceCount;
    AtomicReferenceCount    m_ReferenceCount;

private:
    int                     m_iPriority;
    Reactor                 *m_pReactor;
    ReferenceCountingPolicy  m_RefCntPolicy;
};

#endif

