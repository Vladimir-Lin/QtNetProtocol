#include <netprotocol.h>

N::ScriptableXml:: ScriptableXml ( QObject * parent )
                 : QObject       (           parent )
                 , QScriptable   (                  )
                 , XmlParser     (                  )
{
}

N::ScriptableXml::~ScriptableXml (void)
{
}

void N::ScriptableXml::setValue(QString key,QVariant v)
{
  Variables [ key ] = v ;
}

QVariant N::ScriptableXml::Value(QString key)
{
  return Variables [ key ] ;
}

void N::ScriptableXml::SetEncoding(QString encoding)
{
  setEncoding ( encoding ) ;
}

bool N::ScriptableXml::HasError(void)
{
  return hasError ( ) ;
}

bool N::ScriptableXml::Parse(QString Url,ScriptableByteArray & Body)
{
  QUrl url          ( Url        ) ;
  return ParseLinks ( url , Body ) ;
}

void N::ScriptableXml::AppendTitle(QString title,QByteArray origin)
{
}

void N::ScriptableXml::AppendLink(QString name,QUrl origin,QUrl url)
{
}

void N::ScriptableXml::AppendImage(QString name,QUrl origin,QUrl url)
{
}

void N::ScriptableXml::ReportError(QString text)
{
}

void N::ScriptableXml::AppendString(QString text)
{
}

QScriptValue N::AttacheXml(QScriptContext * context,QScriptEngine * engine)
{
  ScriptableXml * sx = new ScriptableXml ( engine ) ;
  return engine -> newQObject            ( sx     ) ;
}
