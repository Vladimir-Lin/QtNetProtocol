#include <netprotocol.h>

N::UdpDiscovery:: UdpDiscovery (void)
                : NetProtocol  (    )
{
  setParameter ( "Method"     , "None" ) ;
  setParameter ( "Initialize" , false  ) ;
  setParameter ( "Running"    , true   ) ;
  setParameter ( "Type"       , 8964   ) ;
  setParameter ( "State"      , 101    ) ;
  setParameter ( "Interval"   , 10     ) ;
}

N::UdpDiscovery::~UdpDiscovery (void)
{
}

int N::UdpDiscovery::type(void) const
{
  return Parameters [ "Type" ] . toInt ( ) ;
}

bool N::UdpDiscovery::In(int size,char * data)
{
  Buffers [ Pending ] . append ( data , size ) ;
  return Analysis ( )                          ;
}

bool N::UdpDiscovery::In(QByteArray & data)
{
  Buffers [ Pending ] . append ( data ) ;
  return Analysis ( )                   ;
}

bool N::UdpDiscovery::Out(QByteArray & data)
{
  Buffers [ Output ] . append ( data ) ;
  return Analysis ( )                  ;
}

bool N::UdpDiscovery::Actions(void)
{
  if ( ! Parameters [ "Running" ] . toBool ( ) ) return false ;
  Analysis ( )                                                ;
  return Parameters [ "Running" ] . toBool ( )                ;
}

bool N::UdpDiscovery::Analysis(void)
{
  if ( Buffers [ Pending ] . size () > 0 )               {
    Buffers [ Input   ] . append ( Buffers [ Pending ] ) ;
    Buffers [ Pending ] . clear  (                     ) ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  QByteArray LINE                                        ;
  int        code = 0                                    ;
  if ( ( Buffers [ Input  ] . size ( ) == 0 )           &&
       ( Buffers [ Output ] . size ( ) == 0 )          ) {
    Interpret ( code , LINE )                            ;
  }                                                      ;
  while ( ( code = Response ( ) ) > 0 )                  {
    Take      ( LINE        )                            ;
    Tail      ( LINE        )                            ;
    Interpret ( code , LINE )                            ;
  }                                                      ;
  ////////////////////////////////////////////////////////
  return true                                            ;
}

bool N::UdpDiscovery::Interpret(int code,QByteArray & line)
{
  return true ;
}
