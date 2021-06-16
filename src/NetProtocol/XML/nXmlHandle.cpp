#include <netprotocol.h>

N::XmlHandle:: XmlHandle (XmlNode * _node)
{
  this->node = _node ;
}

N::XmlHandle:: XmlHandle (const XmlHandle & ref)
{
  this->node = ref.node;
}

N::XmlHandle::~XmlHandle(void)
{
}

N::XmlHandle N::XmlHandle::operator = (const XmlHandle & ref)
{
  if ( &ref != this ) this->node = ref.node ;
  return *this                              ;
}

N::XmlNode * N::XmlHandle::ToNode(void) const
{
  return node ;
}

N::XmlElement * N::XmlHandle::ToElement(void) const
{
  return ( ( node && node->ToElement() ) ? node->ToElement() : 0 );
}

N::XmlText * N::XmlHandle::ToText(void) const
{
  return ( ( node && node->ToText() ) ? node->ToText() : 0 );
}

N::XmlUnknown * N::XmlHandle::ToUnknown(void) const
{
  return ( ( node && node->ToUnknown() ) ? node->ToUnknown() : 0 );
}

N::XmlNode * N::XmlHandle::Node(void) const
{
  return ToNode();
}

N::XmlElement * N::XmlHandle::Element(void) const
{
  return ToElement();
}

N::XmlText * N::XmlHandle::Text(void) const
{
  return ToText();
}

N::XmlUnknown * N::XmlHandle::Unknown(void) const
{
  return ToUnknown();
}

N::XmlHandle N::XmlHandle::FirstChild(void) const
{
  if ( node )                              {
    XmlNode * child = node -> FirstChild() ;
    if ( child ) return XmlHandle( child ) ;
  }                                        ;
  return XmlHandle ( 0 )                   ;
}

N::XmlHandle N::XmlHandle::FirstChild(const char * value) const
{
  if ( node )                                      {
    XmlNode * child = node -> FirstChild ( value ) ;
    if ( child )        return XmlHandle ( child ) ;
  }                                                ;
  return XmlHandle ( 0 )                           ;
}

N::XmlHandle N::XmlHandle::FirstChildElement(void) const
{
  if ( node )                                        {
    XmlElement * child = node->FirstChildElement ( ) ;
    if ( child ) return XmlHandle ( child )          ;
  }                                                  ;
  return XmlHandle ( 0 )                             ;
}

N::XmlHandle N::XmlHandle::FirstChildElement(const char * value) const
{
  if ( node )                                              {
    XmlElement * child = node->FirstChildElement ( value ) ;
    if ( child )               return XmlHandle  ( child ) ;
  }                                                        ;
  return XmlHandle ( 0 )                                   ;
}

N::XmlHandle N::XmlHandle::Child(int count) const
{
  if ( node )                                {
    int i                                    ;
    XmlNode * child = node->FirstChild ( )   ;
    for (i=0                                 ;
         child && i<count                    ;
         child = child->NextSibling(), ++i ) {
    }                                        ;
    if ( child ) return XmlHandle ( child )  ;
  }                                          ;
  return XmlHandle ( 0 )                     ;
}

N::XmlHandle N::XmlHandle::Child(const char * value,int count) const
{
  if ( node )                                       {
    int i                                           ;
    XmlNode * child = node->FirstChild( value )     ;
    for (i=0                                        ;
         child && i<count                           ;
         child = child->NextSibling( value ), ++i ) {
    }                                               ;
    if ( child ) return XmlHandle ( child )         ;
  }                                                 ;
  return XmlHandle ( 0 )                            ;
}

N::XmlHandle N::XmlHandle::ChildElement(int count) const
{
  if ( node )                                       {
    int i                                           ;
    XmlElement * child = node->FirstChildElement()  ;
    for (i=0                                        ;
         child && i<count                           ;
         child = child->NextSiblingElement(), ++i ) {
    }                                               ;
    if ( child ) return XmlHandle ( child )         ;
  }                                                 ;
  return XmlHandle ( 0 )                            ;
}

N::XmlHandle N::XmlHandle::ChildElement(const char * value,int count) const
{
  if ( node )                                              {
    int i                                                  ;
    XmlElement * child = node->FirstChildElement( value )  ;
    for (i=0                                               ;
         child && i<count                                  ;
         child = child->NextSiblingElement( value ), ++i ) {
    }                                                      ;
    if ( child ) return XmlHandle ( child )                ;
  }                                                        ;
  return XmlHandle ( 0 )                                   ;
}
