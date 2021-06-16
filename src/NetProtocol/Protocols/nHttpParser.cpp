#include <netprotocol.h>

N::HttpParser:: HttpParser (void)
{
}

N::HttpParser:: HttpParser (const HttpParser & parser)
{
  ME = parser ;
}

N::HttpParser:: HttpParser (QByteArray & data)
{
  setHeader ( data ) ;
}

N::HttpParser::~HttpParser (void)
{
  clear ( ) ;
}

N::HttpParser & N::HttpParser::operator = (const HttpParser & parser)
{
  nMemberCopy ( parser , Tags   ) ;
  nMemberCopy ( parser , Header ) ;
  return ME                       ;
}

QStringList N::HttpParser::keys(void) const
{
  return Tags . keys ( ) ;
}

void N::HttpParser::clear(void)
{
  Tags   . clear ( ) ;
  Header . clear ( ) ;
}

int N::HttpParser::setHeader(QByteArray & data)
{
  Header = data                  ;
  Tags   . clear ( )             ;
  if (Header.size()<=0) return 0 ;
  if (hasHttpHeader())           {
    int index = ConfirmHttp()    ;
    while (!EndOfHeader(index))  {
      index = ParseItem(index)   ;
    }                            ;
  }                              ;
  return Tags . count ( )        ;
}

bool N::HttpParser::hasHttpHeader(void)
{
  QByteArray HTTP = Header.left(4) ;
  return (HTTP=="HTTP")            ;
}

int N::HttpParser::ConfirmHttp(void)
{
  QByteArray V                             ;
  QString    v                             ;
  int        index = 4                     ;
  int        endix                         ;
  if (Header.at(index)=='/')               { // has version
    index++                                ;
    endix = Header.indexOf(' ',index)      ;
    if (endix<0) return -1                 ; // incorrect HTTP header
    V = Header.mid(index,endix-index)      ;
    Tags["Version"] = QString::fromUtf8(V) ;
    index = endix + 1                      ;
  }                                        ;
  //////////////////////////////////////////
  index = NextNotBlank(index)              ;
  if (index<0) return -1                   ;
  //////////////////////////////////////////
  endix = Header.indexOf(' ',index)        ;
  V = Header.mid(index,endix-index)        ;
  v = QString::fromUtf8(V)                 ;
  Tags["StatusCode"] = v.toInt()           ;
  //////////////////////////////////////////
  index = endix + 1                        ;
  endix = LineEnd(index)                   ;
  V = Header.mid(index,endix-index)        ;
  v = QString::fromUtf8(V)                 ;
  Tags["StatusMessage"] = v                ;
  //////////////////////////////////////////
  index = endix                            ;
  return index                             ;
}

int N::HttpParser::ParseItem(int index)
{
  QByteArray V                                              ;
  int endix = LineEnd(index)                                ;
  int split = Header.indexOf(':',index)                     ;
  int diffv = 2                                             ;
  if (index>=endix) return -1                               ;
  ///////////////////////////////////////////////////////////
  if (split<0) return endix ; else                          {
    int st = Header.indexOf(": ",index)                     ;
    if (st<0) diffv = 1                                     ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  QString tag                                               ;
  QString result                                            ;
  V      = Header.mid(index,split-index)                    ;
  tag    = QString::fromUtf8(V)                             ;
  split += diffv                                            ;
  V      = Header.mid(split,endix-split)                    ;
  result = QString::fromUtf8(V)                             ;
  if (tag.contains(" "))                                    {
    // ignore space
  } else
  if (tag.compare("Content-Length",Qt::CaseInsensitive)==0) {
    Tags["Content-Length"] = result.toLongLong()            ;
  } else
  if (tag.compare("Content-Type"  ,Qt::CaseInsensitive)==0) {
    Tags["Content-Type"] = result                           ;
  } else                                                    {
    Tags[tag] = result                                      ;
  }                                                         ;
  return endix                                              ;
}

int N::HttpParser::NextNotBlank(int index)
{
  while (index<Header.size() && Header.at(index)==' ') {
    index++                                            ;
  }                                                    ;
  if (index>=Header.size()) return -1                  ;
  return index                                         ;
}

int N::HttpParser::LineEnd(int index)
{
  int i = index                       ;
  do                                  {
    if (Header.at(i)=='\r')           {
      QByteArray el = Header.mid(i,3) ;
      if (el=="\r\n\t") i += 3   ; else
      if (el=="\r\n " ) i += 3   ; else
      return  i                       ;
    }                                 ;
    if (Header.at(i)=='\n')           {
      QByteArray el = Header.mid(i,2) ;
      if (el=="\n\t") i += 2     ; else
      if (el=="\n " ) i += 2     ; else
      return  i                       ;
    }                                 ;
    if (Header.at(i)=='<' )           {
      if (i>0)                        {
        if (Header.at(i-1)=='\n')     {
          return -1                   ;
        }                             ;
      }                               ;
    }                                 ;
    i++                               ;
  } while (i<Header.size())           ;
  return ( Header.size() - 1 )        ;
}

bool N::HttpParser::EndOfHeader(int & index)
{
  if (index< 0            ) return true ;
  if (index>=Header.size()) return true ;
  QByteArray CR   = Header.mid(index,2) ;
  QByteArray CRLF = Header.mid(index,4) ;
  if (CR  =="\n\n"    ) return true     ;
  if (CRLF=="\r\n\r\n") return true     ;
  if (CR  =="\r\n"    )                 {
    index += 2                          ;
    return false                        ;
  }                                     ;
  CR = Header.mid(index,1)              ;
  if (CR=="\n")                         {
    index += 1                          ;
    return false                        ;
  }                                     ;
  return false                          ;
}

int N::HttpParser::statusCode(void) const
{
  if (Tags.contains("StatusCode"))    {
    return Tags["StatusCode"].toInt() ;
  }                                   ;
  return 0                            ;
}

bool N::HttpParser::hasContentType(void) const
{
  if (!Tags.contains("Content-Type")) return false ;
  QString ct = Tags["Content-Type"].toString()     ;
  if (ct.length()<=0) return false                 ;
  return true                                      ;
}

QString N::HttpParser::contentType(void) const
{
  if (!Tags.contains("Content-Type")) return "" ;
  QString CT = Tags["Content-Type"].toString()  ;
  int     ix = CT.indexOf(";")                  ;
  if (ix>0) CT = CT.left(ix)                    ;
  return CT                                     ;
}

bool N::HttpParser::hasMimeType(void)
{
  nKickOut ( !hasContentType()    , false ) ;
  QString     CT = contentType (          ) ;
  QStringList CS = CT.split    ( ';'      ) ;
  nKickOut (  CS.count()<1        , false ) ;
  nKickOut ( !CS[0].contains("/") , false ) ;
  return true                               ;
}

QString N::HttpParser::mimeType(void)
{
  nKickOut ( !hasMimeType() , ""     ) ;
  QString     CT = contentType (     ) ;
  QStringList CS = CT.split    ( ';' ) ;
  return CS[0]                         ;
}

bool N::HttpParser::hasEncoding(void)
{
  nKickOut ( !hasContentType() , false )            ;
  QString CT = contentType ( )                      ;
  return CT.contains("charset",Qt::CaseInsensitive) ;
}

QString N::HttpParser::Encoding(void)
{
  nKickOut ( !hasEncoding() , ""     )             ;
  QString     CT = contentType (     )             ;
  QStringList CS = CT.split    ( ';' )             ;
  for (int i=0;i<CS.count();i++)                   {
    QString C = CS[i]                              ;
    if (C.contains("charset",Qt::CaseInsensitive)) {
      CS = C.split('=')                            ;
      if (CS.count()>1)                            {
        return CS[1]                               ;
      } else return ""                             ;
    }                                              ;
  }                                                ;
  return ""                                        ;
}

bool N::HttpParser::hasContentLength (void) const
{
  if (!Tags.contains("Content-Length")) return false ;
  QString ct = Tags["Content-Length"].toString()     ;
  if (ct.length()<=0) return false                   ;
  return true                                        ;
}

TUID N::HttpParser::contentLength(void) const
{
  if (!Tags.contains("Content-Length")) return 0 ;
  return Tags["Content-Length"].toLongLong()     ;
}

bool N::HttpParser::hasKey(const QString & key) const
{
  return Tags.contains(key) ;
}

bool N::HttpParser::isValid(void) const
{
  return ( Tags.count() > 0 ) ;
}

QString N::HttpParser::toString(void) const
{
  return QString::fromUtf8(Header) ;
}

QString N::HttpParser::value(const QString & key) const
{
  if (Tags.contains(key))       {
    return Tags[key].toString() ;
  }                             ;
  return ""                     ;
}

QDateTime N::HttpParser::toDateTime(const QString & key)
{
  QDateTime DT = QDateTime::currentDateTime()                      ;
  if (Tags.contains(key))                                          {
    QStringList T                                                  ;
    QString s = Tags[key].toString()                               ;
    int     l = s.length()                                         ;
    int     j = s.indexOf(',')                                     ;
    s = s.mid(j+1,l-j-1)                                           ;
    s = s.replace(","    ,"")                                      ;
    s = s.replace(" GMT" ,"")                                      ;
    s = s.replace(" PDT" ,"")                                      ;
    s = s.replace(" KST" ,"")                                      ;
    s = s.replace(" CEST","")                                      ;
    s = s.replace(" CETS","")                                      ;
    s = s.replace(" NZDT","")                                      ;
    T = s.split(" ")                                               ;
    if (T.count()>=4)                                              {
      int   day   = 1                                              ;
      int   month = 1                                              ;
      int   year  = DT.date().year()                               ;
      QTime TTTT  = QTime::currentTime()                           ;
      for (int i=0;i<T.count();i++)                                {
        switch (T[i].length())                                     {
          case 1                                                   :
          case 2                                                   :
            day   = T[i].toInt()                                   ;
          break                                                    ;
          case 3                                                   :
            month = 1                                              ;
            while (month<12 && QDate::shortMonthName(month)!=T[i]) {
              month++                                              ;
            }                                                      ;
          break                                                    ;
          case 4                                                   :
            year  = T[i].toInt()                                   ;
          break                                                    ;
          case 8                                                   :
            TTTT  = QTime::fromString(T[i],"hh:mm:ss")             ;
          break                                                    ;
        }                                                          ;
      }                                                            ;
      QDate DDDD(year,month,day)                                   ;
      DT.setDate(DDDD)                                             ;
      DT.setTime(TTTT)                                             ;
    }                                                              ;
  }                                                                ;
  return DT                                                        ;
}
