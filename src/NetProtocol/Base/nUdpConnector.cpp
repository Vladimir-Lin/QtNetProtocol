#include <netprotocol.h>

N::UdpConnector:: UdpConnector (void)
                : NetConnector (    )
{
  setParameter ( "Protocol" , "UDP" ) ;
}

N::UdpConnector::~UdpConnector (void)
{
}

int N::UdpConnector::type (void) const
{
  return 2 ;
}
