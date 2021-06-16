#include <netprotocol.h>

N::XmlUnknown:: XmlUnknown ( void       )
              : XmlNode    ( xmlUnknown )
{
}

N::XmlUnknown::~XmlUnknown (void)
{
}

N::XmlUnknown:: XmlUnknown ( const XmlUnknown & copy )
              : XmlNode    ( xmlUnknown              )
{
  copy.CopyTo( this );
}

N::XmlUnknown & N::XmlUnknown::operator = ( const XmlUnknown & copy )
{
  copy . CopyTo( this ) ;
  return        *this   ;
}

const N::XmlUnknown * N::XmlUnknown::ToUnknown(void) const
{
  return this;
}

N::XmlUnknown * N::XmlUnknown::ToUnknown(void)
{
  return this;
}

const char * N::XmlUnknown::Parse(const char * p,XmlParsing * data,XML::Encoding encoding)
{
  XmlDocument * document = GetDocument()                           ;
  p = SkipWhiteSpace ( p, encoding )                               ;
  if ( data )                                                      {
    data    -> Stamp( p, encoding )                                ;
    location = data -> Cursor (   )                                ;
  }                                                                ;
  if ( !p || !*p || *p != '<' )                                    {
    if (document)                                                  {
      document->SetError(XmlError_Parsing_Unknown,p,data,encoding) ;
    }                                                              ;
    return NULL                                                    ;
  }                                                                ;
  ++p                                                              ;
  value = ""                                                       ;
  while ( p && *p && *p != '>' )                                   {
    value += *p                                                    ;
    ++p                                                            ;
  }                                                                ;
  if ( !p )                                                        {
    if (document)                                                  {
      document->SetError(XmlError_Parsing_Unknown,0,0   ,encoding) ;
    }                                                              ;
  }                                                                ;
  if ( p && *p == '>' ) return p+1                                 ;
  return p                                                         ;
}

void N::XmlUnknown::CopyTo( XmlUnknown * target ) const
{
  XmlNode :: CopyTo ( target ) ;
}

bool N::XmlUnknown::Accept( XmlVisitor * visitor ) const
{
  return visitor -> Visit ( *this ) ;
}

N::XmlNode * N::XmlUnknown::Clone(void) const
{
  XmlUnknown * clone = new XmlUnknown()   ;
  if ( !clone ) return NULL               ;
  CopyTo ( clone )                        ;
  return   clone                          ;
}
