#include <netprotocol.h>

N::XmlCursor:: XmlCursor (void)
           : row        (-1  )
           , col        (-1  )
{
}

N::XmlCursor::~XmlCursor (void)
{
}

void N::XmlCursor::clear(void)
{
  row = -1 ;
  col = -1 ;
}
