#include <netprotocol.h>

N::UdpService:: UdpService ( void      )
              : UdpServer  (           )
              , Thread     ( 0 , false )
{
  ThreadClass  ( "UdpService"  ) ;
  setParameter ( "Both" , true ) ;
}

N::UdpService::~UdpService (void)
{
}

int N::UdpService::type (void) const
{
  return 12 ;
}

bool N::UdpService::startConnector(NetConnector * connector)
{
  N::VarArgs a1                                      ;
  N::VarArgs a2                                      ;
  bool both = true                                   ;
  int  id                                            ;
  connector -> setParameter ( "State"      , 401   ) ;
  connector -> setCondition ( NetIdle      , false ) ;
  connector -> setCondition ( NetError     , false ) ;
  connector -> setCondition ( NetConnected , true  ) ;
  id = addConnector ( connector )                    ;
  if ( Parameters.contains("Both") )                 {
    both = Parameters [ "Both" ] . toBool ( )        ;
  }                                                  ;
  if ( both )                                        {
    a1 << id                                         ;
    a1 << 3                                          ;
    a1 << true                                       ;
    start ( 10001 , a1 )                             ;
  } else                                             {
    a1 << id                                         ;
    a1 << 1                                          ;
    a1 << true                                       ;
    a1 << id                                         ;
    a1 << 2                                          ;
    a1 << false                                      ;
    start ( 10001 , a1 )                             ;
    start ( 10001 , a2 )                             ;
  }                                                  ;
  return true                                        ;
}

void N::UdpService::run(int Type,ThreadData * data)
{
  int  id       = 0                                     ;
  int  channels = 0                                     ;
  bool deletion = false                                 ;
  switch ( Type )                                       {
    case 10001                                          :
      if ( 3 == data->Arguments.count() )               {
        id       = data -> Arguments [ 0 ] . toInt  ( ) ;
        channels = data -> Arguments [ 1 ] . toInt  ( ) ;
        deletion = data -> Arguments [ 2 ] . toBool ( ) ;
        if ( NotNull(Connectors[id]) )                  {
          Connectors[id] -> AtChannel ( channels )      ;
          if ( deletion ) deleteConnector ( id )        ;
        }                                               ;
      }                                                 ;
    break                                               ;
  }
}

void N::UdpService::run (void)
{
  Listening ( ) ;
}
