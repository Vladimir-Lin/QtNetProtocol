#include <netprotocol.h>

N::TcpService:: TcpService ( void      )
              : TcpServer  (           )
              , Thread     ( 0 , false )
{
  ThreadClass  ( "TcpService"  ) ;
  setParameter ( "Both" , true ) ;
}

N::TcpService::~TcpService (void)
{
}

int N::TcpService::type (void) const
{
  return 11 ;
}

bool N::TcpService::startConnector(NetConnector * connector)
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
    a2 << id                                         ;
    a2 << 2                                          ;
    a2 << false                                      ;
    start ( 10001 , a1 )                             ;
    start ( 10001 , a2 )                             ;
  }                                                  ;
  return true                                        ;
}

void N::TcpService::run(int Type,ThreadData * data)
{
  int  id       = 0                                             ;
  int  channels = 0                                             ;
  bool deletion = false                                         ;
  switch ( Type )                                               {
    case 10001                                                  :
      if ( 3 == data->Arguments.count() )                       {
        id       = data -> Arguments [ 0 ] . toInt  ( )         ;
        channels = data -> Arguments [ 1 ] . toInt  ( )         ;
        deletion = data -> Arguments [ 2 ] . toBool ( )         ;
        if ( NotNull(Connectors[id]) )                          {
          Connectors[id] -> setParameter  ( "Running" , true  ) ;
          Connectors[id] -> AtChannel     ( channels          ) ;
          Connectors[id] -> setParameter  ( "Running" , false ) ;
          if ( deletion ) deleteConnector ( id                ) ;
        }                                                       ;
      }                                                         ;
    break                                                       ;
  }
}

void N::TcpService::run (void)
{
  Listening ( ) ;
}
