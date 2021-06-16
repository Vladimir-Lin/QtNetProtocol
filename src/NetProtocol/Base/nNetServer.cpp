#include <netprotocol.h>

N::NetServer:: NetServer (void)
             : NetTalk   (    )
             , Finder    (NULL)
{
  setParameter ( "Address"   , "0.0.0.0" ) ;
  setParameter ( "Port"      , 0         ) ;
  setParameter ( "Forever"   , false     ) ;
  setParameter ( "Retry"     , 3         ) ;
  setParameter ( "Listening" , false     ) ;
  setParameter ( "Running"   , true      ) ;
  setParameter ( "Stopped"   , true      ) ;
  setParameter ( "Block"     , false     ) ;
}

N::NetServer::~NetServer (void)
{
}

void N::NetServer::Prepared (void)
{
}

void N::NetServer::Started (void)
{
}

void N::NetServer::Accepting (void)
{
  Socket         Incoming                                     ;
  NetConnector * connector                                    ;
  connector = GetConnector ( )                                ;
  if ( NotNull(connector) )                                   {
    if ( Connector . accept ( connector -> Connector ) )      {
      connector -> setParameter                               (
        "Incoming"                                            ,
        connector->Connector.SocketAddress().toString(true) ) ;
      startConnector ( connector )                            ;
    }                                                         ;
  } else
  if ( Connector . accept ( Incoming ) )                      {
    Incoming . close ( )                                      ;
  }                                                           ;
}

void N::NetServer::Finished (void)
{
}

void N::NetServer::setFinder(FindConnector * finder)
{
  if ( NULL != Finder ) {
    delete Finder       ;
  }                     ;
  Finder = finder       ;
}

N::NetConnector * N::NetServer::GetConnector (void)
{
  if ( NotNull(Finder) )           {
    return Finder -> Allocator ( ) ;
  }                                ;
  return NULL                      ;
}

int N::NetServer::addConnector(NetConnector * connector)
{
  QList<int> IDs = Connectors . keys ( ) ;
  int        id  = 0                     ;
  if ( Reversals.contains(connector) )   {
    id = Reversals [ connector ]         ;
    return id                            ;
  }                                      ;
  if (IDs.count()>0) id = IDs.last() + 1 ;
  Connectors [ id        ] = connector   ;
  Reversals  [ connector ] = id          ;
  return id                              ;
}

int N::NetServer::deleteConnector(int id)
{
  if ( !Connectors.contains(id) )     {
    return Connectors . count ( )     ;
  }                                   ;
  NetConnector * connector            ;
  connector = Connectors [ id ]       ;
  if ( NotNull(connector) )           {
    Connectors . remove ( id        ) ;
    Reversals  . remove ( connector ) ;
    delete connector                  ;
  }                                   ;
  return Connectors . count ( )       ;
}

bool N::NetServer::startConnector(NetConnector * connector)
{
  return false ;
}

void N::NetServer::connectorParameter(QString name,QVariant value)
{
  CUIDs IDs = Connectors . keys ( )                    ;
  int   ID                                             ;
  foreach (ID,IDs)                                     {
    Connectors [ ID ] -> setParameter ( name , value ) ;
  }                                                    ;
}

void N::NetServer::connectorCondition(SUID id,bool cond)
{
  CUIDs IDs = Connectors . keys ( )                 ;
  int   ID                                          ;
  foreach (ID,IDs)                                  {
    Connectors [ ID ] -> setCondition ( id , cond ) ;
  }                                                 ;
}

void N::NetServer::Listening (void)
{
  bool      listening = false                                                ;
  bool      duty      = false                                                ;
  bool      sleeping  = true                                                 ;
  bool      blocking  = false                                                ;
  int       state     = 0                                                    ;
  long long usec      = 10 * 1000                                            ;
  QString   protocol                                                         ;
  ////////////////////////////////////////////////////////////////////////////
  duty = Parameters [ "Running"   ] . toBool ( )                             ;
  setParameter ( "Stopped" , false )                                         ;
  setParameter ( "Trying"  , 0     )                                         ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Parameters.contains("State"   ) )                                     {
    state = Parameters [ "State" ] .toInt ( )                                ;
  } else setParameter ( "State" , state ) ;                                  ;
  ////////////////////////////////////////////////////////////////////////////
  while ( duty )                                                             {
    state = Parameters [ "State" ] .toInt ( )                                ;
    switch ( state )                                                         {
      case   0                                                               :
        state    = 101                                                       ;
        sleeping = false                                                     ;
        setParameter ( "Listening" , false )                                 ;
        setParameter ( "State"     , state )                                 ;
        Prepend      (                     )                                 ;
        Prepared     (                     )                                 ;
      break                                                                  ;
      case 101                                                               :
        sleeping = true                                                      ;
        if ( Parameters . contains ( "Block" ) )                             {
          blocking = Parameters [ "Block" ] . toBool ( )                     ;
        } else blocking = false                                              ;
        setParameter        ( "Listening" , false    )                       ;
        protocol  = Parameters [ "Protocol" ] . toString ( )                 ;
        setSocketType ( protocol )                                           ;
        Connector . Prepare ( Address     , blocking )                       ;
        if ( Parameters.contains("Reusable") )                                 {
          Connector . setReusable  ( Parameters [ "Reusable"  ] . toBool ( ) ) ;
        }                                                                      ;
        if ( Parameters . contains ( "Broadcast" ) )                           {
          Connector . setBroadcast ( Parameters [ "Broadcast" ] . toBool ( ) ) ;
        }                                                                      ;
        state = 102                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 102                                                               :
        Connector . setBlock ( true )                                        ;
        if ( Connector . bind ( Address ) )                                  {
          state     = 103                                                    ;
          listening = true                                                   ;
        } else                                                               {
          state = 901                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 103                                                               :
        sleeping = true                                                      ;
        Connector . setBlock ( blocking )                                    ;
        state = 104                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 104                                                               :
        sleeping  = true                                                     ;
        if ( Parameters . contains ( "ListenMethod" ) )                      {
          Connector . setListen ( Parameters [ "ListenMethod" ] . toInt () ) ;
        }                                                                    ;
        setParameter ( "Listening" , true )                                  ;
        state = 105                                                          ;
        setParameter ( "State"     , state )                                 ;
      break                                                                  ;
      case 105                                                               :
        sleeping = true                                                      ;
        Started ( )                                                          ;
        state = 201                                                          ;
        setParameter ( "State"     , state )                                 ;
        setParameter ( "Trying"    , 0     )                                 ;
      break                                                                  ;
      case 201                                                               :
        sleeping = true                                                      ;
        if ( Connector . listen ( ) )                                        {
          state = 202                                                        ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 202                                                               :
        if ( Parameters . contains ( "Detecting" ) )                         {
          usec = Parameters [ "Detecting" ] . toLongLong ( )                 ;
        }                                                                    ;
        if ( Connector . uselect ( usec ) > 0 )                              {
          if ( Connector .isReadable() || Connector.isWritable() )           {
            state = 203                                                      ;
          } else state = 201                                                 ;
        } else state = 201                                                   ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 203                                                               :
        Accepting ( )                                                        ;
        state = 201                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 901                                                               :
        sleeping  = true                                                     ;
        listening = false                                                    ;
        setParameter ( "Listening" , false )                                 ;
        Connector . close ( )                                                ;
        state = 999                                                          ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      case 911                                                               :
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
      case 999                                                               :
        if ( Parameters . contains ( "Running" )                            &&
             Parameters [ "Running" ] . toBool ( ) )                         {
          bool retry   = false                                               ;
          bool Forever = false                                               ;
          ////////////////////////////////////////////////////////////////////
          if ( Parameters . contains ( "Forever" ) )                         {
            Forever = Parameters [ "Forever" ] . toBool ( )                  ;
          }                                                                  ;
          if ( Forever )                                                     {
            retry = true                                                     ;
          } else                                                             {
            if ( Parameters.contains("Retry") )                              {
              if ( Parameters["Trying"].toInt()<Parameters["Retry"].toInt()) {
                retry = true                                                 ;
              }                                                              ;
            }                                                                ;
          }                                                                  ;
          ////////////////////////////////////////////////////////////////////
          if ( retry )                                                       {
            state = 101                                                      ;
            setParameter ( "Trying" , Parameters["Trying"].toInt() + 1 )     ;
            Time :: msleep ( 500 )                                           ;
          } else                                                             {
            duty  = false                                                    ;
          }                                                                  ;
        } else                                                               {
          duty = false                                                       ;
        }                                                                    ;
        setParameter ( "State" , state )                                     ;
      break                                                                  ;
      default                                                                :
      break                                                                  ;
    }                                                                        ;
    if ( sleeping )                                                          {
      Time :: msleep ( Parameters["Interval"] . toInt ( ) )                  ;
    }                                                                        ;
    if ( duty && Parameters.contains("Running") )                            {
      duty = Parameters [ "Running" ] . toBool ( )                           ;
    }                                                                        ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( listening ) Connector . close ( )                                     ;
  Finished ( )                                                               ;
  setParameter ( "Stopped" , true )                                          ;
  ////////////////////////////////////////////////////////////////////////////
  if ( Parameters . contains ( "Deletion" )                                 &&
       Parameters [ "Deletion" ] . toBool ( ) )                              {
    delete this                                                              ;
  }                                                                          ;
}
