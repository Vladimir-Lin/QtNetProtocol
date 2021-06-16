#include <netprotocol.h>

N::XmlComment:: XmlComment ( void       )
              : XmlNode    ( xmlComment )
{
}

N::XmlComment:: XmlComment ( const char * _value )
              : XmlNode    ( xmlComment          )
{
  SetValue ( _value ) ;
}

N::XmlComment:: XmlComment ( const XmlComment & copy )
              : XmlNode    ( xmlComment              )
{
  copy . CopyTo ( this ) ;
}

N::XmlComment::~XmlComment(void)
{
}

N::XmlComment & N::XmlComment::operator = (const XmlComment & base)
{
  Clear         (      ) ;
  base . CopyTo ( this ) ;
  return         *this   ;
}

const N::XmlComment * N::XmlComment::ToComment(void) const
{
  return this ;
}

N::XmlComment * N::XmlComment::ToComment(void)
{
  return this ;
}

const char * N::XmlComment::Parse(const char * p,XmlParsing * data,XML::Encoding encoding)
{
  XmlDocument * document = GetDocument()                           ;
  value = ""                                                       ;
  p     = SkipWhiteSpace( p, encoding )                            ;
  if ( data )                                                      {
    data -> Stamp ( p, encoding )                                  ;
    location = data -> Cursor ( )                                  ;
  }                                                                ;
  const char * startTag = "<!--"                                   ;
  const char * endTag   = "-->"                                    ;
  if ( !StringEqual( p, startTag, false, encoding ) )              {
    if ( document )                                                {
      document->SetError(XmlError_Parsing_Comment,p,data,encoding) ;
    }                                                              ;
    return 0                                                       ;
  }                                                                ;
  p     += strlen ( startTag )                                     ;
  value  = ""                                                      ;
  while (p && *p && !StringEqual( p, endTag, false, encoding ))    {
    value . append ( p, 1 )                                        ;
    ++p                                                            ;
  }                                                                ;
  if ( p && *p ) p += strlen ( endTag )                            ;
  return p                                                         ;
}

void N::XmlComment::CopyTo( XmlComment * target ) const
{
  XmlNode :: CopyTo ( target ) ;
}

bool N::XmlComment::Accept( XmlVisitor * visitor ) const
{
  return visitor -> Visit ( *this ) ;
}

N::XmlNode * N::XmlComment::Clone(void) const
{
  XmlComment * clone = new XmlComment() ;
  if ( !clone ) return NULL             ;
  CopyTo ( clone )                      ;
  return   clone                        ;
}
