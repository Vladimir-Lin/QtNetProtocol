#include <netprotocol.h>

N::DnsProtocol:: DnsProtocol (void)
               : NetProtocol (    )
{
}

N::DnsProtocol::~DnsProtocol (void)
{
}

int N::DnsProtocol::type(void) const
{
  return 1035 ;
}

bool N::DnsProtocol::In(int size,char * data)
{
  return true ;
}

bool N::DnsProtocol::In(QByteArray & data)
{
  return true ;
}

bool N::DnsProtocol::Out(QByteArray & data)
{
  return true ;
}
