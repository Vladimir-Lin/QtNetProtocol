#include <netprotocol.h>

N::XmlElement:: XmlElement ( const char * _value )
              : XmlNode    ( xmlElement          )
{
  firstChild = NULL   ;
  lastChild  = NULL   ;
  value      = _value ;
}

N::XmlElement:: XmlElement ( const XmlElement & copy )
              : XmlNode    (       xmlElement        )
{
  firstChild = NULL      ;
  lastChild  = NULL      ;
  copy . CopyTo ( this ) ;
}

N::XmlElement::~XmlElement(void)
{
  ClearThis ( ) ;
}

N::XmlElement & N::XmlElement::operator = ( const XmlElement & base )
{
  ClearThis     (      ) ;
  base . CopyTo ( this ) ;
  return         *this   ;
}

const N::XmlAttribute * N::XmlElement::FirstAttribute(void) const
{
  return attributeSet . First ( ) ;
}

N::XmlAttribute * N::XmlElement::FirstAttribute(void)
{
  return attributeSet . First ( ) ;
}

const N::XmlAttribute * N::XmlElement::LastAttribute(void) const
{
  return attributeSet . Last ( ) ;
}

N::XmlAttribute * N::XmlElement::LastAttribute(void)
{
  return attributeSet . Last ( ) ;
}

const N::XmlElement * N::XmlElement::ToElement(void) const
{
  return this;
}

N::XmlElement * N::XmlElement::ToElement(void)
{
  return this;
}

bool N::XmlElement::NoEndTag(std::string tag,XML::Encoding encoding)
{
  #define K(key) if (StringEqual(tag.c_str(),key,true,encoding)) return true
  K("meta"  ) ;
  K("area"  ) ;
  K("img"   ) ;
  K("input" ) ;
  K("link"  ) ;
  K("base"  ) ;
  K("br"    ) ;

  K("p"     ) ;
  K("li"    ) ;
  K("frame" ) ;
  K("font"  ) ;
  K("b"     ) ;
  K("u"     ) ;
  K("form"  ) ;
  K("td"    ) ;
  K("tr"    ) ;
  K("th"    ) ;
  K("table" ) ;
  K("center") ;
  #undef  K
  return false ;
}

bool N::XmlElement::IgnoreBalance (std::string tag,XML::Encoding encoding)
{
  #define K(key) if (StringEqual(tag.c_str(),key,true,encoding)) return true
  K("a"     ) ;
  K("b"     ) ;
  K("p"     ) ;
  K("u"     ) ;
  K("font"  ) ;
  K("html"  ) ;
  K("form"  ) ;
  K("td"    ) ;
  K("tr"    ) ;
  K("th"    ) ;
  K("table" ) ;
  K("center") ;
  #undef  K
  return false ;
}

const char * N::XmlElement::Parse(const char * p,XmlParsing * data,XML::Encoding encoding)
{
  XmlDocument * document = GetDocument()                           ;
  p = SkipWhiteSpace( p, encoding )                                ;
  if ( empty( p ) )                                                {
    if ( document )                                                {
      document->SetError(XmlError_Parsing_Element,0,0   ,encoding) ;
    }                                                              ;
    return NULL                                                    ;
  }                                                                ;
  if ( data )                                                      {
    data     -> Stamp          ( p , encoding )                    ;
    location  = data -> Cursor (              )                    ;
  }                                                                ;
  if ( *p != '<' )                                                 {
    if ( document )                                                {
      document->SetError(XmlError_Parsing_Element,p,data,encoding) ;
    }                                                              ;
    return NULL                                                    ;
  }                                                                ;
  p = SkipWhiteSpace ( p+1, encoding )                             ;
  const char * punk = NULL                                         ;
  const char * pErr = p                                            ;
  p = ReadName ( p, &value, encoding )                             ;
  if ( empty ( p ) )                                               {
    if ( document )                                                {
        document->SetError(XmlError_Failed_To_Read_Element_Name    ,
                           pErr, data, encoding                  ) ;
    }                                                              ;
    return NULL                                                    ;
  }                                                                ;
  std::string endTag ("</")                                        ;
  endTag += value                                                  ;
  while ( exists( p ) )                                            {
    pErr = p                                                       ;
    p    = SkipWhiteSpace ( p, encoding )                          ;
    ////////////////////////////////////////////////////////////////
    if ( empty ( p ) )                                             {
      if ( document )                                              {
        document->SetError(XmlError_Reading_Attributes             ,
                           pErr,data,encoding                    ) ;
      }                                                            ;
      return NULL                                                  ;
    }                                                              ;
    ////////////////////////////////////////////////////////////////
    if ( *p == '\"' )                                              {
      p++                                                          ;
    } else
    if ( *p == '\'' )                                              {
      p++                                                          ;
    } else
    if ( *p == '/' )                                               {
      ++p                                                          ;
      if ( *p  != '>' )                                            {
        if ( document )                                            {
          document->SetError(XmlError_Parsing_Empty                ,
                             p,data,encoding                     ) ;
        }                                                          ;
        return NULL                                                ;
      }                                                            ;
      return ( p + 1 )                                             ;
    } else
    if ( *p == '>' )                                               {
      if ( NoEndTag ( value , encoding ) )                         {
        ++p                                                        ;
        return p                                                   ;
      }                                                            ;
      ++p                                                          ;
      punk = p                                                     ;
      p = ReadValue ( p, data, encoding )                          ;
      if ( empty( p ) )                                            {
        if ( document )                                            {
          QString T = QString::fromStdString(value)                ;
          T = T.toLower ( )                                        ;
          if (T=="body") return NULL                               ;
          if (T=="html") return NULL                               ;
          if (T=="head") return NULL                               ;
          document->SetError(XmlError_Reading_End_Tag              ,
                             p,data,encoding                     ) ;
        }                                                          ;
        return NULL                                                ;
      }                                                            ;
      if ( StringEqual( p , endTag.c_str() , false , encoding ) )  {
        p += endTag.length  (             )                        ;
        p  = SkipWhiteSpace ( p, encoding )                        ;
        if ( exists ( p ) && *p == '>' )                           {
          ++p                                                      ;
          return p                                                 ;
        }                                                          ;
        if ( document )                                            {
          QString T = QString::fromStdString(value)                ;
          T = T.toLower ( )                                        ;
          if (T=="body") return NULL                               ;
          if (T=="html") return NULL                               ;
          if (T=="head") return NULL                               ;
          document->SetError(XmlError_Reading_End_Tag              ,
                             p, data, encoding                   ) ;
        }                                                          ;
        return NULL                                                ;
      } else                                                       {
        while (exists(p))                                          {
          if ((*p)=='>')                                           {
            ++p                                                    ;
            return p                                               ;
          } else p++                                               ;
        }                                                          ;
        return NULL                                                ;
      }                                                            ;
    } else                                                         {
      XmlAttribute * attrib = new XmlAttribute ( )                 ;
      if ( !attrib ) return NULL                                   ;
      attrib -> SetDocument ( document )                           ;
      pErr = p                                                     ;
      p    = attrib -> Parse ( p , data , encoding )               ;
      if ( empty ( p ) )                                           {
        if ( document )                                            {
          document->SetError(XmlError_Parsing_Element              ,
                             pErr, data, encoding                ) ;
        }                                                          ;
        delete attrib                                              ;
        return NULL                                                ;
      }                                                            ;
      XmlAttribute * node = attributeSet . Find( attrib->Name() )  ;
      if ( node )                                                  {
        if ( document )                                            {
          document->SetError(XmlError_Parsing_Element              ,
                             pErr, data, encoding                ) ;
        }                                                          ;
        delete attrib                                              ;
        return NULL                                                ;
      }                                                            ;
      attributeSet . Add ( attrib )                                ;
    }                                                              ;
  }                                                                ;
  return p                                                         ;
}

const char * N::XmlElement::ReadValue(const char * p,XmlParsing * data,XML::Encoding encoding)
{
  const char  * startPosition   = p                                   ;
  XmlDocument * document        = GetDocument()                       ;
  const char  * pWithWhiteSpace = p                                   ;
  p = SkipWhiteSpace( p, encoding )                                   ;
  while ( exists ( p ) )                                              {
    if ( *p != '<' )                                                  {
      XmlText * textNode = new XmlText( "" )                          ;
      if ( !textNode ) return NULL                                    ;
      if ( StringEqual(value.c_str(),"script",true,encoding) )        {
        textNode->script_bool = true                                  ;
      }                                                               ;
      if ( XmlBase::IsWhiteSpaceCondensed() )                         {
        p = textNode->Parse ( p              , data, encoding )       ;
      } else                                                          {
        p = textNode->Parse ( pWithWhiteSpace, data, encoding )       ;
      }                                                               ;
      if ( !textNode->Blank() ) LinkEndChild( textNode )              ;
                           else delete textNode                       ;
    } else                                                            {
      if ( StringEqual( p, "</", false, encoding ) ) return p; else   {
        XmlNode * node = Identify ( p, encoding )                     ;
        if ( node )                                                   {
          p = node->Parse ( p, data, encoding )                       ;
          LinkEndChild    ( node              )                       ;
        } else                                                        {
          return NULL                                                 ;
        }                                                             ;
      }                                                               ;
    }                                                                 ;
    pWithWhiteSpace = p                                               ;
    p               = SkipWhiteSpace ( p, encoding )                  ;
  }                                                                   ;
  if ( empty ( p ) )                                                  {
    if ( document )                                                   {
      QString T = QString::fromStdString(value)                       ;
      T = T.toLower ( )                                               ;
      if (T=="body") return p                                         ;
      if (T=="html") return p                                         ;
      if (T=="head") return p                                         ;
      document->SetError(XmlError_Reading_Element_Value,0,0,encoding) ;
    }                                                                 ;
  }                                                                   ;
  return p                                                            ;
}

void N::XmlElement::RemoveAttribute( const char * name )
{
  XmlAttribute * node = attributeSet . Find ( name ) ;
  if ( node )                                        {
    attributeSet . Remove ( node )                   ;
    delete node                                      ;
  }                                                  ;
}

void N::XmlElement::ClearThis(void)
{
  Clear (                      )                   ;
  while ( attributeSet.First() )                   {
    XmlAttribute * node = attributeSet . First ( ) ;
    attributeSet  . Remove ( node )                ;
    delete node                                    ;
  }                                                ;
}

const char * N::XmlElement::Attribute( const char* name ) const
{
  const XmlAttribute * node = attributeSet . Find ( name ) ;
  if ( node ) return node -> Value ( )                     ;
  return NULL                                              ;
}

const char * N::XmlElement::Attribute( const char * name,int * i ) const
{
  const XmlAttribute * attrib = attributeSet . Find ( name ) ;
  const char         * result = 0                            ;
  if ( attrib )                                              {
    result = attrib -> Value         (   )                   ;
    if ( i ) attrib -> QueryIntValue ( i )                   ;
  }                                                          ;
  return result                                              ;
}

const char * N::XmlElement::Attribute( const char * name,double * d ) const
{
  const XmlAttribute * attrib = attributeSet . Find ( name ) ;
  const char         * result = NULL                         ;
  if ( attrib )                                              {
    result = attrib -> Value            (   )                ;
    if ( d ) attrib -> QueryDoubleValue ( d )                ;
  }                                                          ;
  return result                                              ;
}

int N::XmlElement::QueryIntAttribute( const char * name,int * ival ) const
{
  const XmlAttribute * attrib = attributeSet . Find ( name ) ;
  if ( !attrib ) return XML::NoAttribute                     ;
  return attrib -> QueryIntValue ( ival )                    ;
}

int N::XmlElement::QueryUnsignedAttribute( const char * name,unsigned * value ) const
{
  const XmlAttribute * node = attributeSet . Find ( name ) ;
  if ( !node ) return XML::NoAttribute                     ;
  int ival   = 0                                           ;
  int result = node -> QueryIntValue ( &ival )             ;
  *value = (unsigned)ival                                  ;
  return result                                            ;
}

int N::XmlElement::QueryBoolAttribute( const char * name,bool * bval ) const
{
  const XmlAttribute * node = attributeSet . Find ( name )  ;
  if ( !node ) return XML::NoAttribute                      ;
  int result = XML::WrongType                               ;
  if (StringEqual(node->Value(),"true" ,true,XML::Unknown) ||
      StringEqual(node->Value(),"yes"  ,true,XML::Unknown) ||
      StringEqual(node->Value(),"1"    ,true,XML::Unknown)) {
    *bval   = true                                          ;
     result = XML::Success                                  ;
  } else
  if (StringEqual(node->Value(),"false",true,XML::Unknown) ||
      StringEqual(node->Value(),"no"   ,true,XML::Unknown) ||
      StringEqual(node->Value(),"0"    ,true,XML::Unknown)) {
    *bval   = false                                         ;
     result = XML::Success                                  ;
  }                                                         ;
  return result                                             ;
}

int N::XmlElement::QueryDoubleAttribute( const char * name,double * dval ) const
{
  const XmlAttribute * attrib = attributeSet . Find ( name ) ;
  if ( !attrib ) return XML::NoAttribute                     ;
  return attrib -> QueryDoubleValue ( dval )                 ;
}

int N::XmlElement::QueryFloatAttribute(const char * name,float * _value) const
{
  double d                                      ;
  int result = QueryDoubleAttribute ( name,&d ) ;
  if ( result == XML::Success )                 {
    *_value = (float)d                          ;
  }                                             ;
  return result                                 ;
}

void N::XmlElement::SetAttribute( const char * name, int val )
{
  XmlAttribute * attrib = attributeSet . FindOrCreate ( name ) ;
  if ( attrib ) attrib -> SetIntValue ( val )                  ;
}

void N::XmlElement::SetDoubleAttribute( const char * name, double val )
{
  XmlAttribute * attrib = attributeSet . FindOrCreate ( name ) ;
  if ( attrib ) attrib->SetDoubleValue ( val )                 ;
}

void N::XmlElement::SetAttribute( const char * cname, const char * cvalue )
{
  XmlAttribute * attrib = attributeSet . FindOrCreate ( cname ) ;
  if ( attrib ) attrib -> SetValue ( cvalue )                   ;
}

void N::XmlElement::CopyTo( XmlElement * target ) const
{
  XmlNode :: CopyTo ( target )                                 ;
  const XmlAttribute * attribute = 0                           ;
  for (attribute=attributeSet.First()                          ;
       attribute                                               ;
       attribute=attribute->Next())                            {
    target->SetAttribute(attribute->Name(),attribute->Value()) ;
  }                                                            ;
  XmlNode * node = 0                                           ;
  for (node=firstChild;node;node=node->NextSibling())          {
    target -> LinkEndChild ( node->Clone() )                   ;
  }                                                            ;
}

bool N::XmlElement::Accept( XmlVisitor * visitor ) const
{
  if ( visitor -> VisitEnter ( *this , attributeSet.First() ) ) {
    for (const XmlNode * node=FirstChild()                      ;
         node                                                   ;
         node=node->NextSibling()                             ) {
      if ( !node->Accept( visitor ) ) break                     ;
    }                                                           ;
  }                                                             ;
  return visitor -> VisitExit ( *this )                         ;
}

N::XmlNode * N::XmlElement::Clone(void) const
{
  XmlElement * clone = new XmlElement ( Value() ) ;
  if ( !clone ) return NULL                       ;
  CopyTo ( clone )                                ;
  return   clone                                  ;
}

const char * N::XmlElement::GetText(void) const
{
  const XmlNode * child = this->FirstChild()        ;
  if ( child )                                      {
    const XmlText * childText = child -> ToText ( ) ;
    if ( childText ) return childText->Value()      ;
  }                                                 ;
  return NULL                                       ;
}
