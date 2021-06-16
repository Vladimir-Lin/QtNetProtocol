#include <netprotocol.h>

N::UdpBroadcast:: UdpBroadcast (void)
                : Thread       (    )
                , Interpreter  (NULL)
{
  ThreadClass ( "UdpBroadcast" ) ;
}

N::UdpBroadcast::~UdpBroadcast (void)
{
  nEnsureNull ( Interpreter ) ;
}

void N::UdpBroadcast::run(int T,ThreadData * d)
{
  if ( ! IsContinue ( d )  ) return ;
  if ( IsNull(Interpreter) ) return ;
  switch        ( T )               {
    case 10001                      :
      Bind      (   )               ;
    break                           ;
    case 10002                      :
      Broadcast (   )               ;
    break                           ;
    case 10003                      :
      Probe     (   )               ;
    break                           ;
  }                                 ;
}

void N::UdpBroadcast::setNonblock(int s)
{
  #if defined(Q_OS_WIN)
  u_long Flags                           ;
  Flags = 1                              ;
  ::ioctlsocket   ( s                    ,
                    FIONBIO              ,
                    &Flags             ) ;
  #else
  int Flags                              ;
  Flags = ::fcntl ( s                    ,
                    F_GETFL              ,
                    0                  ) ;
  if (0 > Flags) return                  ;
  ::fcntl         ( s                    ,
                    F_SETFL              ,
                    Flags | O_NONBLOCK ) ;
  #endif
}

bool N::UdpBroadcast::uselect(int s,long long usec)
{
  struct timeval tv                ;
  fd_set         rd                ;
  int            rtcode            ;
  //////////////////////////////////
  if ( s < 0 ) return false        ;
  //////////////////////////////////
  FD_ZERO (     &rd )              ;
  FD_SET  ( s , &rd )              ;
  //////////////////////////////////
  tv . tv_sec  = usec / 1000000    ;
  tv . tv_usec = usec % 1000000    ;
  //////////////////////////////////
  rtcode = ::select ( s + 1        ,
                      &rd          ,
                      NULL         ,
                      NULL         ,
                      &tv        ) ;
  //////////////////////////////////
  if (rtcode<0) return false       ;
  if (FD_ISSET(s,&rd)) return true ;
  return false                     ;
}

int N::UdpBroadcast::BroadcastSocket (void)
{
  int s                            ;
  int o = 1                        ;
  int r = 1                        ;
  s = ::socket ( AF_INET           ,
                 SOCK_DGRAM        ,
                 IPPROTO_UDP     ) ;
  ::setsockopt ( s                 ,
                 SOL_SOCKET        ,
                 SO_REUSEADDR      ,
                 (const char *) &r ,
                 sizeof(int)     ) ;
  ::setsockopt ( s                 ,
                 SOL_SOCKET        ,
                 SO_BROADCAST      ,
                 (const char *) &o ,
                 sizeof(int)     ) ;
  return s                         ;
}

void N::UdpBroadcast::Deletion(void)
{
  if ( Interpreter -> Parameters . contains ( "Deletion" ) )     {
    if ( Interpreter -> Parameters [ "Deletion" ] . toBool ( ) ) {
      delete this                                                ;
    }                                                            ;
  }                                                              ;
}

void N::UdpBroadcast::CloseUDP(int s)
{
  #if defined(Q_OS_WIN)
  ::closesocket ( s ) ;
  #else
  ::close       ( s ) ;
  #endif
}

void N::UdpBroadcast::Intermediate(void)
{
  Time :: msleep ( 10 )                                               ;
  int ms = Interpreter -> Parameters [ "Interval" ] . toInt ( )       ;
  if ( ms <= 0 ) return                                               ;
  QDateTime NT = QDateTime::currentDateTime ( )                       ;
  bool      cr                                                        ;
  bool      pe                                                        ;
  do                                                                  {
    cr = true                                                         ;
    if ( ! Interpreter -> Parameters [ "Running" ] . toBool ( ) )     {
      cr = false                                                      ;
    }                                                                 ;
    if ( NT . msecsTo ( QDateTime::currentDateTime() ) > ms )         {
      cr = false                                                      ;
    }                                                                 ;
    pe = true                                                         ;
    if ( Interpreter -> Parameters . contains ( "BlockEvents" ) )     {
      if ( ! Interpreter -> Parameters [ "BlockEvents" ] .toBool()  ) {
        pe = false                                                    ;
      }                                                               ;
    }                                                                 ;
    if ( pe ) qApp -> processEvents ( )                               ;
    if ( cr ) Time :: msleep ( 10 )                                   ;
  } while ( cr )                                                      ;
}

void N::UdpBroadcast::AnyAddress(struct sockaddr_in & a,int port)
{
  a . sin_family        = AF_INET     ;
  a . sin_port          = htons(port) ;
  a . sin_addr . s_addr = INADDR_ANY  ;
}

void N::UdpBroadcast::BroadcastAddress(struct sockaddr_in & a,int port)
{
  a . sin_family        = AF_INET          ;
  a . sin_port          = htons(port)      ;
  a . sin_addr . s_addr = INADDR_BROADCAST ;
}

void N::UdpBroadcast::Probe(void)
{
  int                port   = Interpreter->Parameters ["Port"  ].toInt   ()  ;
  int                state  = Interpreter->Parameters ["State" ].toInt   ()  ;
  int                len    = sizeof(sockaddr_in)                            ;
  ThreadData       * d      = drawer ( )                                     ;
  int                s                                                       ;
  int                size                                                    ;
  QByteArray         E                                                       ;
  char               buff[1024]                                              ;
  struct sockaddr_in a                                                       ;
  struct sockaddr_in b                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  Interpreter -> setParameter ( "Method" , "Probe" )                         ;
  s = BroadcastSocket (          )                                           ;
  AnyAddress          ( a , port                                           ) ;
  BroadcastAddress    ( b , port                                           ) ;
  ::bind              ( s , (sockaddr *) &a , sizeof(sockaddr_in)          ) ;
  setNonblock         ( s        )                                           ;
  while ( Interpreter -> Actions ( )                                        &&
          ( ( NULL == d ) || ( IsContinue ( d ) ) )                        ) {
    switch ( state )                                                         {
      case 101                                                               :
        if ( uselect ( s , 10000 ) )                                         {
          memset(buff,0,1024)                                                ;
          ::recvfrom                                                         (
            s                                                                ,
            buff                                                             ,
            1023                                                             ,
            0                                                                ,
            (struct sockaddr *) & a                                          ,
            &len                                                           ) ;
          if ( strlen(buff) > 0 )                                            {
            QByteArray B ( buff , strlen(buff) )                             ;
            Interpreter -> In ( B )                                          ;
          } else                                                             {
            QByteArray E                                                     ;
            Interpreter -> Interpret ( 0 , E )                               ;
          }                                                                  ;
        }                                                                    ;
        state = 102                                                          ;
        Interpreter -> setParameter ( "State" , state )                      ;
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
      case 102                                                               :
        size = Interpreter->Data(NetProtocol::Output).size()                 ;
        if ( size > 0 )                                                      {
          QByteArray B = Interpreter -> Data ( NetProtocol::Output )         ;
          ::sendto ( s                                                       ,
                     (const char *) B . data ( )                             ,
                     size                                                    ,
                     0                                                       ,
                     (sockaddr *) & b                                        ,
                     len                                                   ) ;
          Interpreter -> Data ( NetProtocol::Output ) . remove ( 0 , size  ) ;
        }                                                                    ;
        state = 101                                                          ;
        Interpreter -> setParameter ( "State" , state )                      ;
        Time :: msleep ( 10 )                                                ;
      break                                                                  ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    Intermediate ( )                                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUDP ( s )                                                             ;
  Deletion (   )                                                             ;
}

void N::UdpBroadcast::Bind(void)
{
  struct sockaddr_in a                                                       ;
  struct sockaddr_in b                                                       ;
  int                s                                                       ;
  int                port  = Interpreter->Parameters [ "Port"  ] . toInt ( ) ;
  int                len   = sizeof(sockaddr_in)                             ;
  ThreadData       * d     = drawer ( )                                      ;
  char               buff [ 1024 ]                                           ;
  QByteArray         E                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  Interpreter -> setParameter ( "Method" , "Listen" )                        ;
  s = BroadcastSocket (                                                    ) ;
  AnyAddress          ( a , port                                           ) ;
  BroadcastAddress    ( b , port                                           ) ;
  ::bind              ( s , (sockaddr *) &a , sizeof(sockaddr_in)          ) ;
  setNonblock         ( s                                                  ) ;
  ////////////////////////////////////////////////////////////////////////////
  while ( Interpreter -> Actions ( )                                        &&
          ( ( NULL == d ) || ( IsContinue ( d ) ) )                        ) {
    if ( uselect ( s , 10000 ) )                                             {
      memset(buff,0,1024)                                                    ;
      ::recvfrom                                                             (
        s                                                                    ,
        buff                                                                 ,
        1023                                                                 ,
        0                                                                    ,
        (struct sockaddr *) & b                                              ,
        &len                                                               ) ;
      if ( strlen(buff) > 0 )                                                {
        QByteArray B ( buff , strlen(buff) )                                 ;
        Interpreter -> In ( B )                                              ;
      }                                                                      ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    Intermediate ( )                                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUDP ( s )                                                             ;
  Deletion (   )                                                             ;
}

void N::UdpBroadcast::Broadcast(void)
{
  ThreadData       * d     = drawer ( )                                      ;
  int                port  = Interpreter->Parameters ["Port"  ] . toInt ( )  ;
  int                state = Interpreter->Parameters ["State" ] . toInt ( )  ;
  int                len   = sizeof(sockaddr_in)                             ;
  int                s                                                       ;
  int                size                                                    ;
  QByteArray         E                                                       ;
  struct sockaddr_in a                                                       ;
  ////////////////////////////////////////////////////////////////////////////
  Interpreter -> setParameter ( "Method" , "Broadcast" )                     ;
  s = BroadcastSocket      (                                               ) ;
  BroadcastAddress         ( a , port                                      ) ;
  setNonblock              ( s                                             ) ;
  Interpreter -> Interpret ( 0 , E                                         ) ;
  while ( Interpreter -> Actions ( )                                        &&
          ( ( NULL == d ) || ( IsContinue ( d ) ) )                        ) {
    size = Interpreter->Data(NetProtocol::Output).size()                     ;
    if ( size > 0 )                                                          {
      QByteArray B = Interpreter -> Data ( NetProtocol::Output )             ;
      ::sendto ( s                                                           ,
                 (const char *) B . data ( )                                 ,
                 size                                                        ,
                 0                                                           ,
                 (sockaddr *) & a                                            ,
                 len                                                       ) ;
    }                                                                        ;
    //////////////////////////////////////////////////////////////////////////
    Intermediate ( )                                                         ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  CloseUDP ( s )                                                             ;
  Deletion (   )                                                             ;
}
