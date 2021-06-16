#include <netprotocol.h>

N::XmlDeclaration:: XmlDeclaration ( void           )
                  : XmlNode        ( xmlDeclaration )
{
}

N::XmlDeclaration:: XmlDeclaration           (
                    const char * _version    ,
                    const char * _encoding   ,
                    const char * _standalone )
                  : XmlNode ( xmlDeclaration )
{
  version    = _version    ;
  encoding   = _encoding   ;
  standalone = _standalone ;
}

N::XmlDeclaration:: XmlDeclaration ( const XmlDeclaration & copy )
                  : XmlNode        ( xmlDeclaration              )
{
  copy . CopyTo ( this ) ;
}

N::XmlDeclaration::~XmlDeclaration(void)
{
}

const char * N::XmlDeclaration::Version(void) const
{
  return version.c_str ();
}

const char * N::XmlDeclaration::Encoding(void) const
{
  return encoding.c_str ();
}

const char * N::XmlDeclaration::Standalone(void) const
{
  return standalone.c_str ();
}

const N::XmlDeclaration * N::XmlDeclaration::ToDeclaration(void) const
{
  return this;
}

N::XmlDeclaration * N::XmlDeclaration::ToDeclaration(void)
{
  return this;
}

const char * N::XmlDeclaration::Parse(const char * p,XmlParsing * data,XML::Encoding _encoding)
{
  XmlDocument * document = GetDocument()                               ;
  p = SkipWhiteSpace ( p, _encoding )                                  ;
  if ( !p || !*p || !StringEqual( p, "<?xml", true, _encoding ) )      {
    if ( document )                                                    {
      document->SetError( XmlError_Parsing_Declaration,0,0,_encoding ) ;
    }                                                                  ;
    return NULL                                                        ;
  }                                                                    ;
  if ( data )                                                          {
    data -> Stamp ( p , _encoding )                                    ;
    location = data -> Cursor (   )                                    ;
  }                                                                    ;
  p          += 5                                                      ;
  version     = ""                                                     ;
  encoding    = ""                                                     ;
  standalone  = ""                                                     ;
  while ( p && *p )                                                    {
    if ( *p == '>' )                                                   {
      ++p                                                              ;
      return p                                                         ;
    }                                                                  ;
    p = SkipWhiteSpace ( p , _encoding )                               ;
    if ( StringEqual ( p , "version" , true , _encoding ) )            {
      XmlAttribute attrib                                              ;
      p          = attrib . Parse ( p , data , _encoding )             ;
      version    = attrib . Value (                      )             ;
    } else
    if ( StringEqual( p , "encoding" , true , _encoding ) )            {
      XmlAttribute attrib                                              ;
      p          = attrib . Parse ( p , data , _encoding )             ;
      encoding   = attrib . Value (                      )             ;
    } else
    if ( StringEqual( p, "standalone", true, _encoding ) )             {
      XmlAttribute attrib                                              ;
      p          = attrib . Parse ( p , data , _encoding )             ;
      standalone = attrib . Value (                      )             ;
    } else                                                             {
      while ( p && *p && *p != '>' && !IsWhiteSpace( *p ) ) ++p        ;
    }                                                                  ;
  }                                                                    ;
  return 0                                                             ;
}

N::XmlDeclaration & N::XmlDeclaration::operator=( const XmlDeclaration & copy )
{
  Clear         (      ) ;
  copy . CopyTo ( this ) ;
  return         *this   ;
}

void N::XmlDeclaration::CopyTo( XmlDeclaration * target ) const
{
  XmlNode :: CopyTo ( target )      ;
  target -> version    = version    ;
  target -> encoding   = encoding   ;
  target -> standalone = standalone ;
}

bool N::XmlDeclaration::Accept( XmlVisitor * visitor ) const
{
  return visitor -> Visit ( *this ) ;
}

N::XmlNode * N::XmlDeclaration::Clone(void) const
{
  XmlDeclaration * clone = new XmlDeclaration() ;
  if ( !clone ) return NULL                     ;
  CopyTo ( clone )                              ;
  return   clone                                ;
}
