#include <netprotocol.h>

N::XmlAttributeSet:: XmlAttributeSet(void)
{
  sentinel.next = &sentinel ;
  sentinel.prev = &sentinel ;
}

N::XmlAttributeSet::~XmlAttributeSet(void)
{
}

void N::XmlAttributeSet::Add(XmlAttribute * addMe)
{
    addMe -> next       = &sentinel        ;
    addMe -> prev       =  sentinel . prev ;
  sentinel . prev->next =  addMe           ;
  sentinel . prev       =  addMe           ;
}

void N::XmlAttributeSet::Remove(XmlAttribute * removeMe)
{
  XmlAttribute * node               ;
  for (node  =  sentinel.next       ;
       node != &sentinel            ;
       node  =  node->next        ) {
    if (node==removeMe)             {
      node->prev->next = node->next ;
      node->next->prev = node->prev ;
      node->next       = 0          ;
      node->prev       = 0          ;
      return                        ;
    }                               ;
  }                                 ;
}

const N::XmlAttribute * N::XmlAttributeSet::First(void) const
{
  return ( sentinel.next == &sentinel ) ? 0 : sentinel . next ;
}

N::XmlAttribute * N::XmlAttributeSet::First(void)
{
  return ( sentinel.next == &sentinel ) ? 0 : sentinel . next ;
}

const N::XmlAttribute * N::XmlAttributeSet::Last(void) const
{
  return ( sentinel.prev == &sentinel ) ? 0 : sentinel . prev ;
}

N::XmlAttribute * N::XmlAttributeSet::Last(void)
{
  return ( sentinel.prev == &sentinel ) ? 0 : sentinel . prev ;
}

N::XmlAttribute * N::XmlAttributeSet::Find(const char* name) const
{
  for (XmlAttribute * node  =  sentinel.next            ;
                      node != &sentinel                 ;
                      node  =  node->next             ) {
    if (strcmp(node->name.c_str(),name)==0) return node ;
  }                                                     ;
  return 0                                              ;
}

N::XmlAttribute * N::XmlAttributeSet::FindOrCreate(const char * _name)
{
  XmlAttribute * attrib = Find(_name) ;
  if (!attrib)                        {
    attrib = new XmlAttribute()       ;
    Add             ( attrib )        ;
    attrib->SetName ( _name  )        ;
  }                                   ;
  return attrib                       ;
}
