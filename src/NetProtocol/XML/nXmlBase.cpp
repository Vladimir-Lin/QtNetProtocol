#include <netprotocol.h>

#define UTF_LEAD_0 0xefU
#define UTF_LEAD_1 0xbbU
#define UTF_LEAD_2 0xbfU

bool N::XmlBase::condenseWhiteSpace = true ;

const int N::XmlBase::utf8ByteTable[256] =
{
// 0 1 2 3 4 5 6 7 8 9 a b c d e f
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x00
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x10
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x20
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x30
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x40
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x50
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x60
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x70	End of ASCII range
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x80 0x80 to 0xc1 invalid
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0x90
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xa0
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,// 0xb0
   1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xc0 0xc2 to 0xdf 2 byte
   2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,// 0xd0
   3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,// 0xe0 0xe0 to 0xef 3 byte
   4,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1 // 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};

N::XmlBase::Entity N::XmlBase::entity[ N::XmlBase::NUM_ENTITY ] =
{
  { "&amp;",  5, '&'  } ,
  { "&lt;",   4, '<'  } ,
  { "&gt;",   4, '>'  } ,
  { "&quot;", 6, '\"' } ,
  { "&apos;", 6, '\'' }
}                       ;

const char * N::XmlBase::errorString[ N::XmlBase::XmlError_String_Count ] =
{
    "No error",
    "Error",
    "Failed to open file",
    "Error parsing Element.",
    "Failed to read Element name",
    "Error reading Element value.",
    "Error reading Attributes.",
    "Error empty tag.",
    "Error reading end tag.",
    "Error parsing Unknown.",
    "Error parsing Comment.",
    "Error parsing Declaration.",
    "Error document empty.",
    "Error null (0) or unexpected EOF found in input stream.",
    "Error parsing CDATA.",
    "Error when TiXmlDocument added to document, because TiXmlDocument can only be at the root.",
};

///////////////////////////////////////////////////////////////////////////////

N::XmlBase:: XmlBase  (void)
           : userData (0   )
{
}

N::XmlBase::~XmlBase (void)
{
}

bool N::XmlBase::empty(const char * p)
{
  if (  p == NULL ) return true ;
  if ( *p == 0    ) return true ;
  return false                  ;
}

bool N::XmlBase::exists(const char * p)
{
  if (  p == NULL ) return false ;
  if ( *p == 0    ) return false ;
  return true                    ;
}

void N::XmlBase::SetCondenseWhiteSpace(bool condense)
{
  condenseWhiteSpace = condense;
}

bool N::XmlBase::IsWhiteSpaceCondensed(void)
{
  return condenseWhiteSpace;
}

int N::XmlBase::Row(void) const
{
  return location.row + 1 ;
}

int N::XmlBase::Column(void) const
{
  return location.col + 1 ;
}

void N::XmlBase::SetUserData(void * user)
{
  userData = user;
}

void * N::XmlBase::GetUserData(void)
{
  return userData;
}

const void * N::XmlBase::GetUserData(void) const
{
  return userData;
}

void N::XmlBase::EncodeString(const std::string & str,std::string * outString)
{
  int i=0;

  while ( i<(int)str.length() ) {
    unsigned char c = (unsigned char) str[i];
    if (c == '&'                       &&
        i  < ( (int)str.length() - 2 ) &&
        str[i+1] == '#'                &&
        str[i+2] == 'x'               ) {
      while ( i<(int)str.length()-1 ) {
        outString->append( str.c_str() + i, 1 );
        ++i;
        if ( str[i] == ';' ) break;
      }
    } else
    if ( c == '&' ) {
      outString->append( entity[0].str, entity[0].strLength );
      ++i;
    } else
    if ( c == '<' ) {
      outString->append( entity[1].str, entity[1].strLength );
      ++i;
    } else
    if ( c == '>' ) {
      outString->append( entity[2].str, entity[2].strLength );
      ++i;
    } else
    if ( c == '\"' ) {
      outString->append( entity[3].str, entity[3].strLength );
      ++i;
    } else
    if ( c == '\'' ) {
      outString->append( entity[4].str, entity[4].strLength );
      ++i;
    } else
    if ( c < 32 ) {
      char buf[ 32 ];
      sprintf( buf, "&#x%02X;", (unsigned) ( c & 0xff ) );
      outString->append( buf, (int)strlen( buf ) );
      ++i;
    } else {
      *outString += (char) c;
      ++i;
    }
  }
}

const char * N::XmlBase::SkipWhiteSpace(const char * p,XML::Encoding encoding)
{
  if ( empty(p) ) return NULL                              ;
  if ( encoding == XML::UTF8 )                             {
    while ( (*p) )                                         {
      const unsigned char * pU = (const unsigned char *)p  ;
      if ((*(pU+0))==UTF_LEAD_0                           &&
          (*(pU+1))==UTF_LEAD_1                           &&
          (*(pU+2))==UTF_LEAD_2                          ) {
        p += 3                                             ;
        continue                                           ;
      } else
      if ((*(pU+0))==UTF_LEAD_0                           &&
          (*(pU+1))==0xbfU                                &&
          (*(pU+2))==0xbeU                               ) {
        p += 3                                             ;
        continue                                           ;
      } else
      if ((*(pU+0))==UTF_LEAD_0                           &&
          (*(pU+1))==0xbfU                                &&
          (*(pU+2))==0xbfU                               ) {
        p += 3                                             ;
        continue                                           ;
      }                                                    ;
      if ( (*p)>0 && IsWhiteSpace( *p ) ) ++p ; else break ;
    }                                                      ;
  } else                                                   {
    while ( (*p)>0 && IsWhiteSpace( *p ) ) ++p             ;
  }                                                        ;
  return p                                                 ;
}

bool N::XmlBase::IsWhiteSpace(char c)
{
  return ( isspace( (unsigned char) c ) || c == '\n' || c == '\r' ) ;
}

bool N::XmlBase::IsWhiteSpace(int c)
{
  if ( c < 256 ) return IsWhiteSpace( (char) c );
  return false;
}

const char * N::XmlBase::ReadName(const char * p,std::string * name,XML::Encoding encoding)
{
  *name = ""                                                                 ;
  if ( p && *p && ( IsAlpha( (unsigned char) *p, encoding ) || *p == '_' ) ) {
    const char * start = p                                                   ;
    while (p && *p && (IsAlphaNum( (unsigned char ) *p, encoding )          ||
           *p == '_' || *p == '-' || *p == '.' || *p == ':'               )) {
      ++p                                                                    ;
    }                                                                        ;
    if ( p-start > 0 ) name->assign( start, p-start )                        ;
    return p                                                                 ;
  }                                                                          ;
  return NULL                                                                ;
}

const char * N::XmlBase::ReadText                  (
        const char            * p                ,
        std::string           * text             ,
        bool                    trimWhiteSpace   ,
        const char            * endTag           ,
        bool                    caseInsensitive  ,
        XML::Encoding encoding         )

{
  *text = "";
  if (!trimWhiteSpace || !condenseWhiteSpace) {
    while ( exists ( p ) && !StringEqual( p, endTag, caseInsensitive, encoding )) {
      int len;
      char cArr[4] = { 0, 0, 0, 0 };
      p = GetChar( p, cArr, &len, encoding );
      text->append( cArr, len );
    }
  } else {
    bool whitespace = false;
    p = SkipWhiteSpace( p, encoding );
    while ( exists ( p ) && !StringEqual( p, endTag, caseInsensitive, encoding )) {
      if ( *p == '\r' || *p == '\n' ) {
        whitespace = true;
        ++p;
      } else
      if ( IsWhiteSpace( *p ) ) {
        whitespace = true;
        ++p;
      } else {
        if ( whitespace ) {
          (*text) += ' ';
          whitespace = false;
        }
        int len;
        char cArr[4] = { 0, 0, 0, 0 };
        p = GetChar( p, cArr, &len, encoding );
        if ( len == 1 ) (*text) += cArr[0];
                   else   text->append( cArr, len );
      }
    }
  }
  if ( exists ( p ) )          {
    int len = strlen( endTag ) ;
    for (int j=0;exists(p) && j<len;j++) p++ ;
  };
  return exists ( p ) ? p : 0;
}

const char * N::XmlBase::GetEntity        (
        const char            * p       ,
        char                  * value   ,
        int                   * length  ,
        XML::Encoding encoding)
{
  std::string ent ;
  int         i   ;

  *length = 0;

  if ( *(p+1) && *(p+1) == '#' && *(p+2) ) {
    unsigned long ucs   = 0 ;
    ptrdiff_t     delta = 0 ;
    unsigned      mult  = 1 ;

    if ( *(p+2) == 'x' ) {

      if ( !*(p+3) ) return 0;

      const char* q = p+3;

      q = strchr( q, ';' );
      if ( !q || !*q ) return 0;
      delta = q-p;
      --q;
      while ( *q != 'x' ) {
        if ( *q >= '0' && *q <= '9' ) ucs += mult * (*q - '0'     ); else
        if ( *q >= 'a' && *q <= 'f' ) ucs += mult * (*q - 'a' + 10); else
        if ( *q >= 'A' && *q <= 'F' ) ucs += mult * (*q - 'A' + 10); else
          return 0;
        mult *= 16;
        --q;
      }
    } else {

      if ( !*(p+2) ) return 0;

      const char* q = p+2;

      q = strchr( q, ';' );
      if ( !q || !*q ) return 0;

      delta = q-p;
      --q;

      while ( *q != '#' ) {
        if ( *q >= '0' && *q <= '9' ) ucs += mult * (*q - '0'); else return 0;
        mult *= 10;
        --q;
      }
    }

    if ( encoding == XML::UTF8 ) {
      ConvertUTF32ToUTF8( ucs, value, length );
    } else {
      *value = (char)ucs;
      *length = 1;
    }
    return p + delta + 1;
  }

  for ( i=0; i<NUM_ENTITY; ++i ) {
    if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 ) {
      *value = entity[i].chr;
      *length = 1;
      return ( p + entity[i].strLength );
    }
  }

  *value = *p;

  return p+1;
}

bool N::XmlBase::StringEqual                 (
        const char            * p          ,
        const char            * tag        ,
        bool                    ignoreCase ,
        XML::Encoding encoding   )
{
  if ( !p || !*p ) return false ;

  const char* q = p;

  if ( ignoreCase ) {
    while ( *q && *tag && ToLower( *q, encoding ) == ToLower(*tag,encoding) ) {
      ++q   ;
      ++tag ;
    }
    if ( *tag == 0 ) return true;
  } else {
    while ( *q && *tag && *q == *tag ) {
      ++q   ;
      ++tag ;
    }
    if ( *tag == 0 ) return true ;
  }
  return false ;
}

const char * N::XmlBase::GetChar(const char * p,char * _value,int * length,XML::Encoding encoding)
{
  if ( encoding == XML::UTF8 ) {
    *length = utf8ByteTable[ *((const unsigned char*)p) ];
  } else {
    *length = 1;
  }

  if ( *length == 1 ) {
    if ( *p == '&' ) return GetEntity (p,_value,length,encoding) ;
    *_value = *p;
    return p+1;
  } else
  if ( *length ) {
    for (int i=0;p[i] && i<*length; ++i) {
      _value[i] = p[i];
    }
    return p + (*length);
  }
  return 0 ;
}

void N::XmlBase::ConvertUTF32ToUTF8(unsigned long input,char * output,int * length)
{
  const unsigned long BYTE_MASK = 0xBF;
  const unsigned long BYTE_MARK = 0x80;
  const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

  if (input < 0x80    ) *length = 1; else
  if (input < 0x800   ) *length = 2; else
  if (input < 0x10000 ) *length = 3; else
  if (input < 0x200000) *length = 4; else {
    *length = 0;
    return;
  }

  output += *length;

  switch (*length) {
    case 4:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 3:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 2:
      --output;
      *output = (char)((input | BYTE_MARK) & BYTE_MASK);
      input >>= 6;
    case 1:
      --output;
      *output = (char)(input | FIRST_BYTE_MARK[*length]);
    break ;
  }
}

int N::XmlBase::IsAlpha(unsigned char anyByte,XML::Encoding encoding)
{
  if ( anyByte < 127 ) return isalpha( anyByte ) ;
  return 1;
}

int N::XmlBase::IsAlphaNum(unsigned char anyByte,XML::Encoding encoding)
{
  if ( anyByte < 127 ) return isalnum( anyByte );
  return 1 ;
}

int N::XmlBase::ToLower(int v,XML::Encoding encoding)
{
  if ( encoding == XML::UTF8 ) {
    if ( v < 128 ) return tolower( v );
    return v;
  }
  return tolower( v );
}
