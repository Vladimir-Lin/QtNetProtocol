#include <netprotocol.h>

N::UdpServer:: UdpServer (void)
             : NetServer (    )
{
  setParameter ( "Protocol" , "UDP" ) ;
}

N::UdpServer::~UdpServer (void)
{
}

int N::UdpServer::type (void) const
{
  return 2 ;
}
