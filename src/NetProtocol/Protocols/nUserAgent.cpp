#include <netprotocol.h>

QStringList N::XML::GetUserAgents(void)
{
  QStringList S                                      ;
  int         i = 0                                  ;
  while ( NULL != XML::UserAgents [ i ] )            {
    S << QString::fromUtf8 ( XML::UserAgents [ i ] ) ;
    i++                                              ;
  }                                                  ;
  return S                                           ;
}

QString N::XML::RandUserAgents (void)
{
  int total = 0                                                 ;
  while ( NULL != XML::UserAgents [ total ] ) total++           ;
  return QString::fromUtf8 ( XML::UserAgents [ rand()%total ] ) ;
}
