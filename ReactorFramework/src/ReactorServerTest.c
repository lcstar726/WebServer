#include "ReactorServerT.h"
#include "ServerAcceptor.h"

typedef ReactorServerT<ServerAcceptor> ReactorServer;

int main( int argc, char* argv[] )
{
    Reactor reactor;

    ReactorServer *svr;

    NEW_RETURN( svr, ReactorServer(argc, argv, &reactor), -1 );

    if( -1 == reactor.RunReactorEventLoop() )
    {
        cout<<__FILE__<<"_"<<__LINE__<<"__reactor.RunReactorEventLoop failed!"<<endl;
    }

    return 0;
}

