#include <netprotocol.h>

N::CiosNeighbors:: CiosNeighbors (QObject * parent)
                 : QObject       (          parent)
{
  TimeOut = 5000                                            ;
  Used    = 0                                               ;
  QObject::connect ( this , SIGNAL ( PrivateChanged ( ) )   ,
                     this , SLOT   ( PrivateChanges ( ) ) ) ;
}

N::CiosNeighbors::~CiosNeighbors (void)
{
}

void N::CiosNeighbors::Clear(void)
{
  Hostnames    . clear ( ) ;
  Applications . clear ( ) ;
  Addresses    . clear ( ) ;
  Status       . clear ( ) ;
  Lastest      . clear ( ) ;
}

QStringList N::CiosNeighbors::Alive(void)
{
  QStringList hostnames = Neighbors ( ) ;
  QStringList addresses                 ;
  QString     host                      ;
  foreach ( host , hostnames )          {
    addresses << Addresses [ host ]     ;
  }                                     ;
  return addresses                      ;
}

QStringList N::CiosNeighbors::Neighbors (void)
{
  QStringList neighbors         ;
  QString     host              ;
  foreach ( host , Hostnames )  {
    if ( 2 != Status [ host ] ) {
      neighbors << host         ;
    }                           ;
  }                             ;
  return neighbors              ;
}

void N::CiosNeighbors::Update(QString address,QString host,QString app)
{ QMutexLocker Locker ( & mutex )        ;
  bool updated = false                   ;
  ////////////////////////////////////////
  if ( ! Hostnames . contains ( host ) ) {
    Hostnames << host                    ;
    updated = true                       ;
  }                                      ;
  ////////////////////////////////////////
  Applications [ host ] = app            ;
  Addresses    [ host ] = address        ;
  Lastest      [ host ] = nTimeNow       ;
  ////////////////////////////////////////
  if ( Status . contains ( host ) )      {
    int status = Status [ host ]         ;
    if ( 2 == status )                   {
      Status [ host ] = 0                ;
    }                                    ;
  } else                                 {
    Status [ host ] = 0                  ;
  }                                      ;
  if ( updated ) Changes ( )             ;
}

void N::CiosNeighbors::Others(QString host,QStringList & addresses)
{ QMutexLocker Locker ( & mutex )     ;
  OtherAddresses [ host ] = addresses ;
}

void N::CiosNeighbors::Verify(void)
{ QMutexLocker Locker ( & mutex )      ;
  bool    updated = false              ;
  QString host                         ;
  //////////////////////////////////////
  foreach ( host , Hostnames )         {
    QDateTime T                        ;
    int       dT                       ;
    T  = Lastest [ host ]              ;
    dT = T . msecsTo ( nTimeNow )      ;
    if ( dT > TimeOut )                {
      Status [ host ] = 2              ;
      updated = true                   ;
    }                                  ;
  }                                    ;
  //////////////////////////////////////
  if ( updated ) Changes ( )           ;
}

void N::CiosNeighbors::Take(QString host)
{ QMutexLocker Locker ( & mutex ) ;
  int index                       ;
  index = Hostnames.indexOf(host) ;
  if (index>=0)                   {
    Hostnames.takeAt(index)       ;
  }                               ;
  Applications . take ( host )    ;
  Addresses    . take ( host )    ;
  Status       . take ( host )    ;
  Lastest      . take ( host )    ;
}

void N::CiosNeighbors::PrivateChanges (void)
{
  emit Changed ( ) ;
}

void N::CiosNeighbors::Changes(void)
{
  emit PrivateChanged ( ) ;
}
