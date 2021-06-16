#include <netprotocol.h>

N::XmlText:: XmlText ( const char * initValue )
           : XmlNode ( xmlText                )
{
  SetValue ( initValue ) ;
  cdata       = false    ;
  script_bool = false    ;
}

N::XmlText:: XmlText ( const XmlText & copy )
           : XmlNode ( xmlText              )
{
  copy . CopyTo( this );
}

N::XmlText::~XmlText(void)
{
}

N::XmlText & N::XmlText::operator = ( const XmlText & base )
{
  base . CopyTo ( this ) ;
  return         *this   ;
}

bool N::XmlText::CDATA(void) const
{
  return cdata;
}

void N::XmlText::SetCDATA(bool _cdata)
{
  cdata = _cdata;
}

const N::XmlText * N::XmlText::ToText(void) const
{
  return this;
}

N::XmlText * N::XmlText::ToText(void)
{
  return this;
}

const char * N::XmlText::Parse(const char * p,XmlParsing * data,XML::Encoding encoding)
{
  XmlDocument * document = GetDocument()                           ;
  value = ""                                                       ;
  if ( data )                                                      {
    data     -> Stamp          ( p, encoding )                     ;
    location  = data -> Cursor (             )                     ;
  }                                                                ;
  const char * const startTag = "<![CDATA["                        ;
  const char * const endTag   = "]]>"                              ;
  if ( cdata || StringEqual(p,startTag,false,encoding) )           {
    cdata = true                                                   ;
    if ( !StringEqual(p,startTag,false,encoding) )                 {
      if ( document )                                              {
        document->SetError(XmlError_Parsing_CDATA,p,data,encoding) ;
      }                                                            ;
      return 0                                                     ;
    }                                                              ;
    p += strlen ( startTag )                                       ;
    while (p && *p && !StringEqual( p, endTag, false, encoding ))  {
      value += *p                                                  ;
      ++p                                                          ;
    }                                                              ;
    std::string dummy                                              ;
    p = ReadText (p,&dummy,false,endTag,false,encoding)            ;
    return p                                                       ;
  } else                                                           {
    bool         ignoreWhite = true                                ;
    const char * end         = "<"                                 ;
    if (this->script_bool)                                         {
      end = "</"                                                   ;
      p   = ReadText(p,&value,ignoreWhite,end,true ,encoding)      ;
      if ( exists ( p ) ) p--                                      ;
    } else                                                         {
      p   = ReadText(p,&value,ignoreWhite,end,false,encoding)      ;
    }                                                              ;
    if ( exists ( p ) ) return p - 1                               ;
  }                                                                ;
  return NULL                                                      ;
}

bool N::XmlText::Blank(void) const
{
  for ( unsigned i=0; i<value.length(); i++ ) {
    if (!IsWhiteSpace(value[i])) return false ;
  }                                           ;
  return true                                 ;
}

void N::XmlText::CopyTo( XmlText * target ) const
{
  XmlNode :: CopyTo ( target ) ;
  target   -> cdata = cdata    ;
}

bool N::XmlText::Accept( XmlVisitor * visitor ) const
{
  return visitor -> Visit ( *this ) ;
}

N::XmlNode * N::XmlText::Clone(void) const
{
  XmlText * clone = NULL    ;
  clone = new XmlText( "" ) ;
  if ( !clone ) return NULL ;
  CopyTo ( clone )          ;
  return   clone            ;
}
