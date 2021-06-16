#include <netprotocol.h>

N::XmlParser:: XmlParser   (void)
             : ErrorCode   (0   )
             , ErrorString (""  )
             , ErrorRow    (0   )
             , ErrorColumn (0   )
{
  Codec = QTextCodec::codecForName("UTF8") ;
}

N::XmlParser::~XmlParser (void)
{
}

bool N::XmlParser::hasError(void) const
{
  return ( ErrorCode != 0 ) ;
}

bool N::XmlParser::ParseLinks(QUrl & Url,QByteArray & Body)
{
  XmlDocument XML                       ;
  XML.Parse(Body)                       ;
  if (XML.Error())                      {
    QString E(XML.ErrorDesc())          ;
    ErrorCode   = XML.ErrorId()         ;
    ErrorString = E                     ;
    ErrorRow    = XML.ErrorRow()        ;
    ErrorColumn = XML.ErrorCol()        ;
    ReportError (E)                     ;
  }                                     ;
  XmlElement * root = XML.RootElement() ;
  return ParseElement ( Url , root )    ;
}

bool N::XmlParser::ParseElement(QUrl & Url,XmlNode * element)
{
  if (IsNull(element)) return false         ;
  HandleElement ( Url , element )           ;
  if (element->Type()!=XmlNode::xmlElement) {
    return true                             ;
  }                                         ;
  XmlNode * node                            ;
  for (node = element -> FirstChild  ()     ;
       node                                 ;
       node = node    -> NextSibling ()   ) {
    ParseElement ( Url , node )             ;
  }                                         ;
  return true                               ;
}

void N::XmlParser::HandleElement(QUrl & Url,XmlNode * element)
{
  if (element->Type()!=XmlNode::xmlElement) return  ;
  XmlElement * p = (XmlElement *)element            ;
  if (p->isTag("title"))                            {
    QByteArray TB(p->GetText())                     ;
    QString title = Codec->toUnicode(p->GetText())  ;
    AppendTitle ( title , TB )                      ;
  } else
  if (p->isTag("a"))                                {
    QString name = Codec->toUnicode(p->GetText())   ;
    QString href(p->Attribute("href"))              ;
    QUrl ah (href)                                  ;
    if (href.length()>0) AppendLink(name,Url,ah)    ;
  } else
  if (p->isTag("link"))                             {
    QString name = Codec->toUnicode(p->GetText())   ;
    QString href(p->Attribute("href"))              ;
    QUrl ah (href)                                  ;
    if (href.length()>0) AppendLink(name,Url,ah)    ;
  } else
  if (p->isTag("img"))                              {
    QString name = Codec->toUnicode(p->GetText())   ;
    QString src(p->Attribute("src"))                ;
    QUrl ah (src)                                   ;
    if (src.length()>0) AppendImage(name,Url,ah)    ;
  } else                                            {
  }                                                 ;
}

void N::XmlParser::setEncoding(QString encoding)
{
  QTextCodec * codec = QTextCodec::codecForName(encoding.toUtf8()) ;
  if (IsNull(codec)) return                                        ;
  Codec = codec                                                    ;
}

void N::XmlParser::AppendTitle(QString title,QByteArray origin)
{
}

void N::XmlParser::AppendLink(QString name,QUrl origin,QUrl url)
{
}

void N::XmlParser::AppendImage(QString name,QUrl origin,QUrl url)
{
}

void N::XmlParser::ReportError(QString text)
{
}

void N::XmlParser::AppendString(QString text)
{
}
