#include <netprotocol.h>

N::NetTalk:: NetTalk     (void)
           : Interpreter (NULL)
{
  setParameter ( "Block"      , true             ) ;
  setParameter ( "Size"       , 0                ) ;
  setParameter ( "LineMax"    , 1024             ) ;
  setParameter ( "Timeout"    ,  1 * 1000 * 1000 ) ;
  setParameter ( "Connecting" , 30 * 1000 * 1000 ) ;
  setParameter ( "Silence"    , 10 * 1000 * 1000 ) ;
  setCondition ( NetIdle      , true             ) ;
  setCondition ( NetError     , false            ) ;
  setCondition ( NetConnected , false            ) ;
  setCondition ( NetLogin     , false            ) ;
  setCondition ( NetLogout    , false            ) ;
  setCondition ( NetReading   , false            ) ;
  setCondition ( NetWriting   , false            ) ;
}

N::NetTalk::~NetTalk (void)
{
}

bool N::NetTalk::Lookup(QString hostname)
{
  Alternates . clear ( )                             ;
  if (!N::Lookup(hostname,Alternates) ) return false ;
  if ( Alternates . count ( ) <= 0    ) return false ;
  Address = Alternates [ 0 ]                         ;
  return true                                        ;
}

void N::NetTalk::setParameter(QString name,QVariant value)
{
  Parameters [ name ] = value ;
}

void N::NetTalk::setCondition(SUID id,bool cond)
{
  Decision . setCondition ( id , cond ) ;
}

bool N::NetTalk::Reset(void)
{
  if (Connector.isConnected())   {
    Connector . close ( )        ;
  }                              ;
  return true                    ;
}

void N::NetTalk::setSocketType (QString P)
{
  if (P=="TCP")                             {
    Connector . setSocketType ( TCP       ) ;
  } else
  if (P=="UDP")                             {
    Connector . setSocketType ( UDP       ) ;
  } else
  if (P=="RAW")                             {
    Connector . setSocketType ( RAW       ) ;
  } else
  if (P=="RDM")                             {
    Connector . setSocketType ( RDM       ) ;
  } else
  if (P=="SEQPACKET")                       {
    Connector . setSocketType ( SEQPACKET ) ;
  }                                         ;
}

bool N::NetTalk::Connect(void)
{
  if (Parameters.contains("Port"))                                       {
    Address . setPort ( Parameters [ "Port" ] . toInt ( ) )              ;
  }                                                                      ;
  bool block = true                                                      ;
  if (Parameters.contains("Block"))                                      {
    block = Parameters [ "Block" ] . toBool ( )                          ;
  }                                                                      ;
  TUID tout   = 30 * 1000 * 1000                                         ;
  if (Parameters.contains("Connecting"))                                 {
    tout = Parameters [ "Connecting" ] . toLongLong ( )                  ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  QString P = "TCP"                                                      ;
  if (Parameters.contains("Protocol"))                                   {
    P = Parameters [ "Protocol" ] . toString ( )                         ;
  }                                                                      ;
  setSocketType ( P )                                                    ;
  ////////////////////////////////////////////////////////////////////////
  Connector . Prepare       ( Address , block )                          ;
  Connector . setTimeOut    ( tout            )                          ;
  ////////////////////////////////////////////////////////////////////////
  if ( Parameters.contains("Reusable") )                                 {
    Connector . setReusable  ( Parameters [ "Reusable"  ] . toBool ( ) ) ;
  }                                                                      ;
  if ( Parameters . contains ( "Broadcast" ) )                           {
    Connector . setBroadcast ( Parameters [ "Broadcast" ] . toBool ( ) ) ;
  }                                                                      ;
  ////////////////////////////////////////////////////////////////////////
  setCondition ( NetIdle      , true  )                                  ;
  setCondition ( NetError     , false )                                  ;
  setCondition ( NetConnected , false )                                  ;
  setCondition ( NetLogin     , false )                                  ;
  setCondition ( NetLogout    , false )                                  ;
  setCondition ( NetReading   , false )                                  ;
  setCondition ( NetWriting   , false )                                  ;
  ////////////////////////////////////////////////////////////////////////
  block = Connector . open ( QIODevice::ReadWrite )                      ;
  if ( ! block ) return false                                            ;
  if (Parameters.contains("Timeout"))                                    {
    tout = Parameters [ "Timeout" ] . toLongLong ( )                     ;
  }                                                                      ;
  Connector . setTimeOut ( tout )                                        ;
  return true                                                            ;
}

bool N::NetTalk::isConnected(TUID usec)
{
  bool connected = false                                        ;
  if ( 0 == usec ) connected = Connector . isConnected (      ) ;
              else connected = Connector . isConnected ( usec ) ;
  bool isError = Connector . isError ( )                        ;
  if ( connected )                                              {
    setCondition ( NetIdle      , false   )                     ;
    setCondition ( NetError     , isError )                     ;
    setCondition ( NetConnected , true    )                     ;
  } else                                                        {
    setCondition ( NetIdle      , true    )                     ;
    setCondition ( NetError     , isError )                     ;
    setCondition ( NetConnected , false   )                     ;
  }                                                             ;
  return connected                                              ;
}

bool N::NetTalk::Close(void)
{
  Connector . close ( )                   ;
  if ( ! Connector . isConnected ( )  )   {
    setCondition ( NetIdle      , true  ) ;
    setCondition ( NetError     , false ) ;
    setCondition ( NetConnected , false ) ;
    setCondition ( NetLogin     , false ) ;
    setCondition ( NetLogout    , false ) ;
    setCondition ( NetReading   , false ) ;
    setCondition ( NetWriting   , false ) ;
    return true                           ;
  }                                       ;
  return false                            ;
}

bool N::NetTalk::Decide(void)
{
  UUIDs react                                ;
  react = Decision . reactions ( )           ;
  if ( react . count ( ) <= 0 ) return false ;
  return Process ( react )                   ;
}

bool N::NetTalk::Process(UUIDs & reactions)
{
  return true ;
}

bool N::NetTalk::ReadChunk(void)
{
  int        Size = 0                             ;
  QByteArray B                                    ;
  if (Parameters.contains("Size"))                {
    Size = Parameters [ "Size" ] . toInt ( )      ;
  }                                               ;
  if ( Size <= 0 ) return false                   ;
  /////////////////////////////////////////////////
  B = Connector . read ( Size )                   ;
  if ( B . size ( ) <= 0 ) return false           ;
  /////////////////////////////////////////////////
  if (NotNull(Interpreter))                       {
    Interpreter -> In ( B )                       ;
  }                                               ;
  /////////////////////////////////////////////////
  return true                                     ;
}

bool N::NetTalk::ReadLine(void)
{
  int        LineMax = 1024                        ;
  QByteArray B                                     ;
  if (Parameters.contains("LineMax"))              {
    LineMax = Parameters [ "LineMax" ] . toInt ( ) ;
  }                                                ;
  //////////////////////////////////////////////////
  B = Connector . readLine ( LineMax )             ;
  if ( B . size ( ) <= 0 ) return false            ;
  //////////////////////////////////////////////////
  if (NotNull(Interpreter))                        {
    Interpreter -> In ( B )                        ;
  }                                                ;
  //////////////////////////////////////////////////
  return true                                      ;
}

bool N::NetTalk::ReadStructure(void)
{
  return false ;
}

bool N::NetTalk::WriteCommand(void)
{
  if ( IsNull(Interpreter)) return false                                 ;
  if ( Interpreter->Data(NetProtocol::Output).size() <= 0 ) return false ;
  while ( Interpreter -> Data ( NetProtocol::Output ) . size ( ) > 0 )   {
    Connector . uselect ( 1000 )                                         ;
    if (   Connector . isError    ( ) ) return false                     ;
    if ( ! Connector . isWritable ( ) ) continue                         ;
    int r                                                                ;
    r = Connector . writeTo ( Interpreter->Data( NetProtocol::Output ) ) ;
    if ( r > 0 )                                                         {
      Interpreter -> Data ( NetProtocol::Output ) . remove ( 0 , r )     ;
    }                                                                    ;
  }                                                                      ;
  return true                                                            ;
}

bool N::NetTalk::Until(void)
{
  return ( Coda < nTimeNow ) ;
}

void N::NetTalk::setUntil(int msec)
{
  Coda = nTimeNow . addMSecs ( msec ) ;
}

void N::NetTalk::SyncTime(void)
{
  if ( IsNull(Interpreter) ) return                ;
  if ( Interpreter -> Coda > Coda                ) {
    Coda              = Interpreter->Coda          ;
  } else
  if ( Coda                > Interpreter -> Coda ) {
    Interpreter->Coda = Coda                       ;
  }                                                ;
}

void N::NetTalk::AccessTime(void)
{
  setUntil ( Parameters [ "Timeout" ] . toLongLong ( ) / 1000 ) ;
}

void N::NetTalk::Intermediate (bool & sleeping,bool & duty)
{
  if ( sleeping )                                         {
    Time :: msleep ( Parameters["Interval"] . toInt ( ) ) ;
  }                                                       ;
  if ( duty && Parameters.contains("Running") )           {
    duty = Parameters [ "Running" ] . toBool ( )          ;
  }                                                       ;
  if ( duty )                                             {
    if ( Until ( ) ) duty = false                         ;
  }                                                       ;
}

void N::NetTalk::AutoDeletion (void)
{
  if ( Parameters . contains ( "Deletion" )    &&
       Parameters [ "Deletion" ] . toBool ( ) ) {
    delete this                                 ;
  }                                             ;
}

void N::NetTalk::Prepend(void)
{
  if ( Parameters.contains("Address" ) )                               {
    Address . setAddress ( Parameters [ "Address" ] . toString  ( )  ) ;
  }                                                                    ;
  if ( Parameters.contains("Port"    ) )                               {
    Address . setPort    ( Parameters [ "Port"    ] . toInt     ( )  ) ;
  }                                                                    ;
  if ( Parameters.contains("Protocol") )                               {
    setSocketType ( Parameters [ "Protocol" ] . toString ( )         ) ;
  }                                                                    ;
  if ( ! Parameters.contains("Interval") )                             {
    setParameter ( "Interval" , 10 )                                   ;
  }                                                                    ;
}
