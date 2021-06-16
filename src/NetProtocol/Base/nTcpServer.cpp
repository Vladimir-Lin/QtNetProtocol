#include <netprotocol.h>

N::TcpServer:: TcpServer (void)
             : NetServer (    )
{
  setParameter ( "Protocol" , "TCP" ) ;
}

N::TcpServer::~TcpServer (void)
{
}

int N::TcpServer::type (void) const
{
  return 1 ;
}
