#include <netprotocol.h>

#define UTF_LEAD_0 0xefU
#define UTF_LEAD_1 0xbbU
#define UTF_LEAD_2 0xbfU

N::XmlDocument:: XmlDocument ( void         )
               : XmlNode     (  xmlDocument )
{
  tabsize         = 4     ;
  useMicrosoftBOM = false ;
  ClearError ( )          ;
}

N::XmlDocument:: XmlDocument( const char * documentName )
               : XmlNode    ( xmlDocument               )
{
  tabsize         = 4            ;
  useMicrosoftBOM = false        ;
  value           = documentName ;
  ClearError ( )                 ;
}

N::XmlDocument:: XmlDocument( const XmlDocument & copy )
               : XmlNode    ( xmlDocument              )
{
  copy.CopyTo( this );
}

N::XmlDocument::~XmlDocument(void)
{
}

const N::XmlElement * N::XmlDocument::RootElement(void) const
{
  return FirstChildElement();
}

N::XmlElement * N::XmlDocument::RootElement(void)
{
  return FirstChildElement();
}

bool N::XmlDocument::Error(void) const
{
  return error;
}

const char * N::XmlDocument::ErrorDesc(void) const
{
  return errorDesc . c_str ( ) ;
}

int N::XmlDocument::ErrorId(void) const
{
  return errorId ;
}

int N::XmlDocument::ErrorRow(void) const
{
  return errorLocation.row + 1 ;
}

int N::XmlDocument::ErrorCol(void) const
{
  return errorLocation.col + 1 ;
}

void N::XmlDocument::SetTabSize(int _tabsize)
{
  tabsize = _tabsize ;
}

int N::XmlDocument::TabSize(void) const
{
  return tabsize;
}

void N::XmlDocument::ClearError(void)
{
  error              = false ;
  errorId            = 0     ;
  errorDesc          = ""    ;
  errorLocation.row  = 0     ;
  errorLocation.col  = 0     ;
//  errorLocation.last = 0     ;
}

const N::XmlDocument * N::XmlDocument::ToDocument(void) const
{
  return this;
}

N::XmlDocument * N::XmlDocument::ToDocument(void)
{
  return this;
}

const char * N::XmlDocument::Parse(QByteArray & Body)
{
  return Parse ( Body.constData() , NULL , XML::DefaultEncoding ) ;
}

const char * N::XmlDocument::Parse(const char * p,XmlParsing * prevData,XML::Encoding encoding)
{
  ClearError       ( )                                             ;
  location . clear ( )                                             ;
  //////////////////////////////////////////////////////////////////
  // No data input                                                //
  //////////////////////////////////////////////////////////////////
  if (empty(p))                                                    {
    SetError( XmlError_Document_Empty,0,0,XML::Unknown )           ;
    return NULL                                                    ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  if ( prevData )                                                  {
    location . row = prevData -> cursor . row                      ;
    location . col = prevData -> cursor . col                      ;
  } else                                                           {
    location . row = 0                                             ;
    location . col = 0                                             ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  XmlParsing data( p, TabSize(), location.row, location.col )      ;
  location = data . Cursor ( )                                     ;
  //////////////////////////////////////////////////////////////////
  // Unicode verification                                         //
  //////////////////////////////////////////////////////////////////
  if ( encoding == XML::Unknown )                                  {
    const unsigned char * pU = (const unsigned char*)p             ;
    if (*(pU+0) && *(pU+0) == UTF_LEAD_0                          &&
        *(pU+1) && *(pU+1) == UTF_LEAD_1                          &&
        *(pU+2) && *(pU+2) == UTF_LEAD_2                         ) {
      encoding        = XML::UTF8                                  ;
      useMicrosoftBOM = true                                       ;
    }                                                              ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  p = SkipWhiteSpace( p, encoding )                                ;
  if ( !p )                                                        {
    SetError( XmlError_Document_Empty,0,0,XML::Unknown )           ;
    return NULL                                                    ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  while (exists(p))                                                {
    XmlNode * node = Identify ( p, encoding )                      ;
    if ( node )                                                    {
      p = node -> Parse ( p, &data, encoding )                     ;
      LinkEndChild      ( node               )                     ;
    } else break                                                   ;
    ////////////////////////////////////////////////////////////////
    if (encoding==XML::Unknown && node->ToDeclaration())           {
      XmlDeclaration * dec = node -> ToDeclaration ()              ;
      const char     * enc = dec  -> Encoding      ()              ;
      if ( *enc == 0 ) encoding = XML::UTF8; else
      if ( StringEqual(enc, "UTF-8", true, XML::Unknown) )
        encoding = XML::UTF8; else
      if ( StringEqual(enc, "UTF8" , true, XML::Unknown) )
        encoding = XML::UTF8; else
        encoding = XML::Legacy                                     ;
    }                                                              ;
    p = SkipWhiteSpace ( p , encoding )                            ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  if ( !firstChild )                                               {
    SetError ( XmlError_Document_Empty,0,0,encoding )              ;
    return NULL                                                    ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  return p                                                         ;
}

void N::XmlDocument::SetError(int err,const char * pError,XmlParsing * data,XML::Encoding encoding)
{
  if ( error ) return                 ;

  error     = true                    ;
  errorId   = err                     ;
  errorDesc = errorString [ errorId ] ;

  errorLocation . clear ()            ;

  if ( pError && data )               {
    data->Stamp( pError, encoding )   ;
    errorLocation = data->Cursor( )   ;
  }                                   ;
}

N::XmlDocument & N::XmlDocument::operator=( const XmlDocument & copy )
{
  Clear         (      ) ;
  copy . CopyTo ( this ) ;
  return *this           ;
}

void N::XmlDocument::CopyTo( XmlDocument * target ) const
{
  XmlNode :: CopyTo ( target )                ;

  target -> error           = error           ;
  target -> errorId         = errorId         ;
  target -> errorDesc       = errorDesc       ;
  target -> tabsize         = tabsize         ;
  target -> errorLocation   = errorLocation   ;
  target -> useMicrosoftBOM = useMicrosoftBOM ;

  XmlNode * node = NULL                       ;
  for ( node = firstChild                     ;
        node                                  ;
        node = node->NextSibling()          ) {
    target -> LinkEndChild ( node->Clone()  ) ;
  }                                           ;
}

N::XmlNode * N::XmlDocument::Clone(void) const
{
  XmlDocument * clone = new XmlDocument() ;
  if ( !clone ) return NULL               ;
  CopyTo ( clone )                        ;
  return clone                            ;
}

bool N::XmlDocument::Accept( XmlVisitor * visitor ) const
{
  if ( visitor -> VisitEnter ( *this ) )    {
    for (const XmlNode * node=FirstChild()  ;
         node                               ;
         node=node->NextSibling()         ) {
      if ( !node->Accept( visitor ) ) break ;
    }                                       ;
  }                                         ;
  return visitor -> VisitExit ( *this )     ;
}
