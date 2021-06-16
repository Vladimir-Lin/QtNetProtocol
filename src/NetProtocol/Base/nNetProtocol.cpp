#include <netprotocol.h>

N::NetProtocol:: NetProtocol (void)
{
  Carry = "\n"                           ;
//  Carry = "\r\n"                         ;
  Decisions . Blank ( 0                ) ;
  setBuffer         ( Pending  , 0     ) ;
  setBuffer         ( Input    , 0     ) ;
  setBuffer         ( Output   , 0     ) ;
  setCondition      ( CanRead  , false ) ;
  setCondition      ( CanWrite , false ) ;
}

N::NetProtocol::~NetProtocol (void)
{
}

int N::NetProtocol::setBuffer (int index,int size)
{
  QByteArray Body                    ;
  Buffers[index] = Body              ;
  if (size>0)                        {
    Buffers[index] . resize ( size ) ;
  }                                  ;
  return Buffers[index] . size ()    ;
}

void N::NetProtocol::setParameter(QString name,QVariant value)
{
  Parameters [ name ] = value ;
}

void N::NetProtocol::setCondition(SUID id,bool cond)
{
  Decisions [ 0 ] . setCondition ( id , cond ) ;
}

QByteArray & N::NetProtocol::Data(int index)
{
  if (!Buffers.contains(index)) {
    setBuffer ( index , 0 )     ;
  }                             ;
  return Buffers [ index ]      ;
}

bool N::NetProtocol::contains(QString key)
{
  return Parameters . contains ( key ) ;
}

QVariant & N::NetProtocol::value(QString key)
{
  return Parameters [ key ] ;
}

void N::NetProtocol::Place(QString command)
{
  QString X = command + Carry                    ;
  Buffers [ Output ] . append ( X . toUtf8 ( ) ) ;
}

int N::NetProtocol::Response(void)
{
  int loc = Buffers [ Input ] . indexOf ( '\n' ) ;
  if ( loc < 0 ) return -1                       ;
  int s = Buffers [ Input ] . size ( )           ;
  int v = 0                                      ;
  int i = 0                                      ;
  while ( i < s )                                {
    QChar c = Buffers [ Input ] . at ( i )       ;
    if ( c >= '0' && c <= '9' )                  {
      v *= 10                                    ;
      v += c . digitValue ( )                    ;
    } else                                       {
      if ( i <= 0 ) v = -1                       ;
      setParameter ( "Response" , v )            ;
      return v                                   ;
    }                                            ;
    i++                                          ;
  }                                              ;
  return v                                       ;
}

bool N::NetProtocol::Take(QByteArray & line)
{
  int loc = Buffers [ Input ] . indexOf ( '\n' )              ;
  line . clear ( )                                            ;
  if ( loc < 0 ) return false                                 ;
  const char * d = (const char *)Buffers [ Input ] . data ( ) ;
  line . append ( d , loc + 1 )                               ;
  Buffers [ Input ] . remove ( 0 , loc + 1 )                  ;
  return ( line . size ( ) > 0 )                              ;
}

bool N::NetProtocol::Tail(QByteArray & line)
{
  int loc = line . indexOf ( ' ' )                        ;
  if ( loc < 0 ) return false                             ;
  while ( loc < line.size() && line . at ( loc ) == ' ' ) {
    loc++                                                 ;
  }                                                       ;
  line . remove  ( 0 , loc )                              ;
  QString L = QString::fromUtf8(line)                     ;
  L    = L . replace ( "\r" , "" )                        ;
  L    = L . replace ( "\n" , "" )                        ;
  line = L . toUtf8  (           )                        ;
  return line . size (           ) > 0                    ;
}

bool N::NetProtocol::Interpret(int code,QByteArray & line)
{
  return true ;
}

bool N::NetProtocol::Actions(void)
{
  return true ;
}

bool N::NetProtocol::Decide(void)
{
  UUIDs react                                ;
  react = Decisions [ 0 ] . reactions ( )    ;
  if ( react . count ( ) <= 0 ) return false ;
  return Process ( react )                   ;
}

bool N::NetProtocol::Process(UUIDs & reactions)
{
  return true ;
}
