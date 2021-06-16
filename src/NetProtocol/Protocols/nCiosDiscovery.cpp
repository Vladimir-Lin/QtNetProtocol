#include <netprotocol.h>

N::CiosDiscovery:: CiosDiscovery (void)
                 : UdpDiscovery  (    )
                 , neighbors     (NULL)
{
  setParameter ( "Myself" , false ) ;
}

N::CiosDiscovery::~CiosDiscovery (void)
{
  if ( NotNull(neighbors) )     {
    if ( neighbors->Used <= 0 ) {
      delete neighbors          ;
    } else                      {
      neighbors->Used--         ;
    }                           ;
    neighbors = NULL            ;
  }                             ;
}

bool N::CiosDiscovery::Interpret(int code,QByteArray & line)
{
  QString m                                                                  ;
  switch ( code )                                                            {
    case 0                                                                   :
      if ( ! Parameters [ "Initialize" ] . toBool   ( ) )                    {
        if ( Parameters [ "Method"     ] . toString ( ) == "Listen"    )     {
          setParameter ( "Initialize" , true       )                         ;
          setParameter ( "Interval"   , 30         )                         ;
        } else
        if ( Parameters [ "Method"     ] . toString ( ) == "Probe"     )     {
          setParameter ( "Initialize" , true       )                         ;
          setParameter ( "Trying"     , 15         )                         ;
          PlaceAddress ( 100                       )                         ;
        } else
        if ( Parameters [ "Method"     ] . toString ( ) == "Broadcast" )     {
          setParameter ( "Initialize" , true       )                         ;
          setParameter ( "Interval"   , 100        )                         ;
          PlaceAddress ( 300                       )                         ;
        } else                                                               {
        }                                                                    ;
      } else                                                                 {
        if ( Parameters [ "Method"     ] . toString ( ) == "Probe"     )     {
          int trying = Parameters["Trying"].toInt()                          ;
          if ( trying > 0 )                                                  {
            setParameter ( "Trying"   , trying - 1 )                         ;
            PlaceAddress ( 100                     )                         ;
          } else                                                             {
            setParameter ( "Running"  , false      )                         ;
          }                                                                  ;
        } else
        if ( Parameters [ "Method"     ] . toString ( ) == "Broadcast" )     {
          PlaceAddress   ( 300                     )                         ;
        } else
        if ( Parameters [ "Method"     ] . toString ( ) == "Listen"    )     {
          neighbors -> Verify ( )                                            ;
        }                                                                    ;
      }                                                                      ;
    break                                                                    ;
    case 100                                                                 :
      m = QString::fromUtf8(line)                                            ;
      PlaceAddress  ( 200 )                                                  ;
    break                                                                    ;
    case 200                                                                 :
      m = QString::fromUtf8(line)                                            ;
      AcceptAddress ( m   )                                                  ;
    break                                                                    ;
    case 300                                                                 :
      m = QString::fromUtf8(line)                                            ;
      AcceptAddress ( m   )                                                  ;
      PlaceAddress  ( 200 )                                                  ;
    break                                                                    ;
    default                                                                  :
    break                                                                    ;
  }                                                                          ;
  return true                                                                ;
}

QString N::CiosDiscovery::LocalAddress(void)
{
  return QString ( "%1:%2"                                  )
         . arg   ( Parameters [ "Address" ] . toString( )   )
         . arg   ( Parameters [ "Port"    ] . toInt   ( ) ) ;
}

void N::CiosDiscovery::PlaceAddress (int code)
{
  if ( Data ( Output ) . size ( ) > 0 ) return       ;
  QStringList s                                      ;
  QString     m                                      ;
  s   << QString::number ( code )                    ;
  s   << LocalAddress    (      )                    ;
  s   << Parameters [ "Hostname"    ] . toString ( ) ;
  s   << Parameters [ "Application" ] . toString ( ) ;
  if ( Parameters . contains ( "Addresses" ) )       {
    s << Parameters [ "Addresses"   ] . toString ( ) ;
  }                                                  ;
  m  = s . join ( " " )                              ;
  Place         ( m   )                              ;
}

void N::CiosDiscovery::AcceptAddress (QString address)
{
  if ( address.length() <= 0 ) return      ;
  QStringList s = address . split ( ' ' )  ;
  if ( s . count ( ) < 3 ) return          ;
  if ( s [ 0 ] == LocalAddress ( ) )       {
    setParameter ( "Myself" , true )       ;
    neighbors -> Verify ( )                ;
  } else                                   {
    QString addr = s[0]                    ;
    QString host = s[1]                    ;
    QString apps = s[2]                    ;
    if ( s . count ( ) > 3 )               {
      for (int i=0;i<3;i++)                {
        s . takeFirst ( )                  ;
      }                                    ;
      neighbors -> Others ( host , s )     ;
    }                                      ;
    neighbors -> Update ( addr,host,apps ) ;
    setParameter ( "Accept" , addr )       ;
  }                                        ;
}
