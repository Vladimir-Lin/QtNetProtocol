#include <netprotocol.h>

N::ScriptableHttp:: ScriptableHttp ( QObject * parent )
                  : QObject        (           parent )
                  , QScriptable    (                  )
                  , HttpParser     (                  )
{
}

N::ScriptableHttp::~ScriptableHttp (void)
{
}

void N::ScriptableHttp::Clear(void)
{
  clear ( ) ;
}

int N::ScriptableHttp::SetHeader(ScriptableByteArray & data)
{
  return setHeader ( data ) ;
}

bool N::ScriptableHttp::HasHttpHeader(void)
{
  return hasHttpHeader ( ) ;
}

int N::ScriptableHttp::IsConfirmHttp(void)
{
  return ConfirmHttp ( ) ;
}

int N::ScriptableHttp::DoParseItem(int index)
{
  return ParseItem ( index ) ;
}

bool N::ScriptableHttp::IsEndOfHeader(int & index)
{
  return EndOfHeader ( index ) ;
}

int N::ScriptableHttp::GetNextNotBlank(int index)
{
  return NextNotBlank ( index ) ;
}

int N::ScriptableHttp::GetLineEnd(int index)
{
  return LineEnd ( index ) ;
}

int N::ScriptableHttp::StatusCode(void) const
{
  return statusCode ( ) ;
}

bool N::ScriptableHttp::HasContentType(void) const
{
  return hasContentType ( ) ;
}

QString N::ScriptableHttp::ContentType(void) const
{
  return contentType ( ) ;
}

bool N::ScriptableHttp::HasMimeType(void)
{
  return hasMimeType ( ) ;
}

QString N::ScriptableHttp::MimeType(void)
{
  return mimeType ( ) ;
}

bool N::ScriptableHttp::HasEncoding(void)
{
  return hasEncoding ( ) ;
}

QString N::ScriptableHttp::GetEncoding(void)
{
  return Encoding ( ) ;
}

TUID N::ScriptableHttp::ContentLength(void) const
{
  return contentLength ( ) ;
}

bool N::ScriptableHttp::HasContentLength(void) const
{
  return hasContentLength ( ) ;
}

bool N::ScriptableHttp::IsValid(void) const
{
  return isValid ( ) ;
}

QString N::ScriptableHttp::ToString(void) const
{
  return toString ( ) ;
}

int N::ScriptableHttp::KeysCount(void)
{
  return Tags . count ( ) ;
}

void N::ScriptableHttp::setValue(QString key,QString v)
{
  Tags [ key ] = v ;
}

bool N::ScriptableHttp::HasKey(const QString key) const
{
  return hasKey ( key ) ;
}

QString N::ScriptableHttp::Value(const QString key) const
{
  return value ( key ) ;
}

QDateTime N::ScriptableHttp::ToDateTime(const QString key)
{
  return toDateTime ( key ) ;
}

QScriptValue N::AttacheHttp(QScriptContext * context,QScriptEngine * engine)
{
  ScriptableHttp * sh = new ScriptableHttp ( engine ) ;
  return engine -> newQObject              ( sh     ) ;
}
