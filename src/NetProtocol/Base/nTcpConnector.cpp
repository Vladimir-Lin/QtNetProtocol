#include <netprotocol.h>

N::TcpConnector:: TcpConnector (void)
                : NetConnector (    )
{
  setParameter ( "Protocol" , "TCP" ) ;
}

N::TcpConnector::~TcpConnector (void)
{
}

int N::TcpConnector::type (void) const
{
  return 1 ;
}
