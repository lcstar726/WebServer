#ifndef __FUNCTOR_T_H__
#define __FUNCTOR_T_H__

#include "Public.h"

class CommandBase
{
public:
    CommandBase( void ) {}
    virtual ~CommandBase( void );
    virtual int Execute( void *arg = NULL ) = 0;
};

template<class RECEIVER, class ACTION>
class CommandCallback: public CommandBase
{
public:
    CommandCallback( RECEIVER &receiver, ACTION action );
    virtual ~CommandCallback( void );
    virtual int Execute( void *arg = NULL );

private:
    RECEIVER    &m_Receiver;
    ACTION      m_Action;
};

template<class RECEIVER, class ACTION>
CommandCallback<RECEIVER, ACTION>::CommandCallback( RECEIVER &receiver, ACTION action )
    : m_Receiver( receiver ), m_Action( action )
{
}
template<class RECEIVER, class ACTION>
CommandCallback<RECEIVER, ACTION>::~CommandCallback( void )
{
}
template<class RECEIVER, class ACTION>
int CommandCallback<RECEIVER, ACTION>::Execute( void *arg )
{
    return (this->m_Receiver.*this->m_Action)( arg );
}

template<class RECEIVER>
class MemberFunctionCommandT: public CommandBase
{
public:
    typedef void (RECEIVER::*PTMF)( void );

    MemberFunctionCommandT( RECEIVER &recv, PTMF ptmf );
    virtual ~MemberFunctionCommandT( void );

    virtual int Execute( void *arg = NULL );

private:
    RECEIVER    &m_rReceiver;
    PTMF        m_pMemberFunc;
};

template<class RECEIVER>
MemberFunctionCommandT<RECEIVER>::MemberFunctionCommandT( RECEIVER &recv, PTMF ptmf )
    : m_rReceiver( recv ), m_pMemberFunc( ptmf )
{
}
template<class RECEIVER>
MemberFunctionCommandT<RECEIVER>::~MemberFunctionCommandT( void )
{
}

template<class RECEIVER>
int MemberFunctionCommandT<RECEIVER>::Execute( void *arg )
{
    ( this->m_rReceiver.*this->m_pMemberFunc )();
    return 0;
}

#endif

