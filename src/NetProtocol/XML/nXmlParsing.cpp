#include <netprotocol.h>

#define UTF_LEAD_0 0xefU
#define UTF_LEAD_1 0xbbU
#define UTF_LEAD_2 0xbfU

N::XmlParsing:: XmlParsing (void)
{
}

N::XmlParsing::~XmlParsing (void)
{
}

const N::XmlCursor & N::XmlParsing::Cursor(void) const
{
  return cursor;
}

N::XmlParsing::XmlParsing(const char * start,int _tabsize,int row,int col)
{
  stamp      =  start   ;
  tabsize    = _tabsize ;
  cursor.row =  row     ;
  cursor.col =  col     ;
}

void N::XmlParsing::Stamp(const char * now,XML::Encoding encoding)
{
  if ( tabsize < 1 ) return       ;
  /////////////////////////////////
  int          row = cursor . row ;
  int          col = cursor . col ;
  const char * p   = stamp        ;
  /////////////////////////////////
  while ( p < now ) {
    const unsigned char* pU = (const unsigned char*)p;
    switch ((*pU)) {
      case 0:
      return;
      case '\r':
        ++row;
        col = 0;
        ++p;
        if (*p == '\n') ++p ;
      break;
      case '\n':
        ++row;
        col = 0;
        ++p;
        if (*p == '\r') ++p;
      break;
      case '\t':
        ++p;
        col = (col / tabsize + 1) * tabsize;
      break;
      case UTF_LEAD_0:
        if ( encoding == XML::UTF8 ) {
          if ( *(p+1) && *(p+2) ) {
            if ( *(pU+1)==UTF_LEAD_1 && *(pU+2)==UTF_LEAD_2 ) p += 3; else
            if ( *(pU+1)==0xbfU      && *(pU+2)==0xbeU      ) p += 3; else
            if ( *(pU+1)==0xbfU      && *(pU+2)==0xbfU      ) p += 3; else {
              p +=3 ;
              ++col ;
            };
          };
        } else {
          ++p   ;
          ++col ;
        };
      break;
      default:
        if ( encoding == XML::UTF8 ) {
          int step = XmlBase::utf8ByteTable[*((const unsigned char*)p)];
          if ( step == 0 ) step = 1;
          p += step;
          ++col;
        } else {
          ++p   ;
          ++col ;
        };
      break;
    };
  };

  cursor . row = row ;
  cursor . col = col ;
  stamp        = p   ;
}
