#include <netprotocol.h>

N::XmlNode:: XmlNode (void)
           : XmlBase (    )
{
}

N::XmlNode:: XmlNode (NodeType _type)
           : XmlBase (              )
{
  type       = _type ;
  parent     = NULL  ;
  firstChild = NULL  ;
  lastChild  = NULL  ;
  prev       = NULL  ;
  next       = NULL  ;
}

N::XmlNode::~XmlNode(void)
{
  XmlNode * node = firstChild ;
  XmlNode * temp = NULL       ;
  while ( node )              {
    temp = node               ;
    node = node->next         ;
    delete temp               ;
  }                           ;
}

bool N::XmlNode::isTag(QString tag)
{
  return ( Tag ( ) . toLower ( ) == tag . toLower ( ) ) ;
}

QString N::XmlNode::Tag(void) const
{
  return QString :: fromStdString ( value ) ;
}

const char * N::XmlNode::Value(void) const
{
  return value.c_str ( ) ;
}

const std::string & N::XmlNode::ValueTStr(void) const
{
  return value;
}

void N::XmlNode::SetValue(const char * _value)
{
  value = _value;
}

N::XmlNode * N::XmlNode::Parent(void)
{
  return parent;
}

const N::XmlNode * N::XmlNode::Parent(void) const
{
  return parent;
}

const N::XmlNode * N::XmlNode::FirstChild(void) const
{
  return firstChild;
}

N::XmlNode * N::XmlNode::FirstChild(void)
{
  return firstChild;
}

N::XmlNode * N::XmlNode::FirstChild(const char * _value)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->FirstChild(_value));
}

const N::XmlNode * N::XmlNode::LastChild(void) const
{
  return lastChild;
}

N::XmlNode * N::XmlNode::LastChild(void)
{
  return lastChild;
}

N::XmlNode * N::XmlNode::LastChild(const char * _value)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->LastChild(_value));
}

N::XmlNode * N::XmlNode::IterateChildren(const XmlNode * previous)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->IterateChildren(previous));
}

N::XmlNode * N::XmlNode::IterateChildren(const char * _value,const XmlNode* previous)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->IterateChildren(_value,previous));
}

const N::XmlNode * N::XmlNode::PreviousSibling(void) const
{
  return prev;
}

N::XmlNode * N::XmlNode::PreviousSibling(void)
{
  return prev;
}

N::XmlNode * N::XmlNode::PreviousSibling(const char *_prev)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->PreviousSibling(_prev));
}

const N::XmlNode * N::XmlNode::NextSibling(void) const
{
  return next;
}

N::XmlNode * N::XmlNode::NextSibling(void)
{
  return next;
}

N::XmlNode * N::XmlNode::NextSibling(const char * _next)
{
  return const_cast<XmlNode*>((const_cast<const XmlNode*>(this))->NextSibling(_next));
}

N::XmlElement * N::XmlNode::NextSiblingElement(void)
{
  return const_cast<XmlElement*>((const_cast<const XmlNode*>(this))->NextSiblingElement());
}

N::XmlElement * N::XmlNode::NextSiblingElement(const char *_next)
{
  return const_cast<XmlElement*>((const_cast<const XmlNode*>(this))->NextSiblingElement(_next));
}

N::XmlElement * N::XmlNode::FirstChildElement(void)
{
  return const_cast<XmlElement*>((const_cast<const XmlNode*>(this))->FirstChildElement());
}

N::XmlElement * N::XmlNode::FirstChildElement(const char * _value)
{
  return const_cast<XmlElement*>((const_cast<const XmlNode*>(this))->FirstChildElement(_value));
}

int N::XmlNode::Type(void) const
{
  return type;
}

N::XmlDocument * N::XmlNode::GetDocument(void)
{
  return const_cast<XmlDocument*>((const_cast<const XmlNode*>(this))->GetDocument());
}

bool N::XmlNode::NoChildren(void) const
{
  return !firstChild;
}

const N::XmlDocument * N::XmlNode::ToDocument(void) const
{
  return NULL;
}

const N::XmlElement * N::XmlNode::ToElement(void) const
{
  return NULL;
}

const N::XmlComment * N::XmlNode::ToComment(void) const
{
  return NULL;
}

const N::XmlUnknown * N::XmlNode::ToUnknown(void) const
{
  return NULL;
}

const N::XmlText * N::XmlNode::ToText(void) const
{
  return NULL;
}

const N::XmlDeclaration * N::XmlNode::ToDeclaration(void) const
{
  return NULL;
}

N::XmlDocument * N::XmlNode::ToDocument(void)
{
  return NULL;
}

N::XmlElement * N::XmlNode::ToElement(void)
{
  return NULL;
}

N::XmlComment * N::XmlNode::ToComment(void)
{
  return NULL;
}

N::XmlUnknown * N::XmlNode::ToUnknown(void)
{
  return NULL;
}

N::XmlText * N::XmlNode::ToText(void)
{
  return NULL;
}

N::XmlDeclaration * N::XmlNode::ToDeclaration(void)
{
  return NULL;
}

void N::XmlNode::CopyTo( XmlNode * target ) const
{
  target -> SetValue (value.c_str() ) ;
  target -> userData = userData       ;
  target -> location = location       ;
}

void N::XmlNode::Clear(void)
{
  XmlNode * node = firstChild ;
  XmlNode * temp = NULL       ;
  while ( node )              {
    temp = node               ;
    node = node->next         ;
    delete temp               ;
  }                           ;
  firstChild = NULL           ;
  lastChild  = NULL           ;
}

N::XmlNode * N::XmlNode::LinkEndChild( XmlNode * node )
{
  if ( node->Type() == xmlDocument )      {
    delete node                           ;
    if ( GetDocument() )                  {
      GetDocument()->SetError             (
        XmlError_Document_Top_Only        ,
        0, 0, XML::Unknown    ) ;
    }                                     ;
    return NULL                           ;
  }                                       ;
  /////////////////////////////////////////
  node->parent = this                     ;
  node->prev   = lastChild                ;
  node->next   = NULL                     ;
  /////////////////////////////////////////
  if ( lastChild ) lastChild->next = node ;
              else firstChild      = node ;
  lastChild = node                        ;
  /////////////////////////////////////////
  return      node                        ;
}

N::XmlNode * N::XmlNode::InsertEndChild(const XmlNode & addThis)
{
  if ( addThis.Type() == xmlDocument ) {
    if ( GetDocument() )
         GetDocument()->SetError( XmlError_Document_Top_Only,0,0,XML::Unknown );
      return 0;
  }
  XmlNode * node = addThis.Clone();
  if ( !node ) return NULL;
  return LinkEndChild ( node ) ;
}

N::XmlNode * N::XmlNode::InsertBeforeChild(XmlNode * beforeThis,const XmlNode & addThis)
{
  if ( !beforeThis || beforeThis->parent != this ) return 0 ;
  if ( addThis.Type() == xmlDocument ) {
    if ( GetDocument() )
         GetDocument()->SetError( XmlError_Document_Top_Only,0,0,XML::Unknown );
    return 0;
  }

  XmlNode * node = addThis.Clone() ;
  if ( !node ) return NULL;

  node -> parent = this             ;
  node -> next   = beforeThis       ;
  node -> prev   = beforeThis->prev ;
  if ( beforeThis->prev ) beforeThis->prev->next = node ;
                     else firstChild             = node ;
  beforeThis->prev = node;
  return node;
}

N::XmlNode * N::XmlNode::InsertAfterChild( XmlNode * afterThis,const XmlNode & addThis)
{
  if ( !afterThis || afterThis->parent != this ) return 0 ;
  if ( addThis.Type() == xmlDocument ) {
    if ( GetDocument() )
         GetDocument()->SetError( XmlError_Document_Top_Only,0,0,XML::Unknown ) ;
    return 0;
  }

  XmlNode * node = addThis.Clone();
  if ( !node ) return NULL;

  node -> parent = this            ;
  node -> prev   = afterThis       ;
  node -> next   = afterThis->next ;

  if ( afterThis->next ) afterThis->next->prev = node ;
                    else lastChild             = node ;
  afterThis->next = node;
  return node;
}

N::XmlNode * N::XmlNode::ReplaceChild( XmlNode * replaceThis,const XmlNode & withThis)
{
  if ( !replaceThis                 ) return 0 ;
  if (  replaceThis->parent != this ) return 0 ;
  if ( withThis.ToDocument()        ) {
    XmlDocument * document = GetDocument();
    if ( document )
         document->SetError( XmlError_Document_Top_Only,0,0,XML::Unknown );
    return NULL;
  }

  XmlNode * node = withThis.Clone();
  if ( !node ) return NULL;

  node->next = replaceThis->next ;
  node->prev = replaceThis->prev ;

  if ( replaceThis->next ) replaceThis->next->prev = node ;
                      else lastChild               = node ;
  if ( replaceThis->prev ) replaceThis->prev->next = node ;
                      else firstChild              = node ;

  delete replaceThis;
  node->parent = this;

  return node ;
}

bool N::XmlNode::RemoveChild( XmlNode * removeThis )
{
  if ( !removeThis                 ) return false ;
  if (  removeThis->parent != this ) return false ;
  if (  removeThis->next           ) removeThis->next->prev = removeThis->prev ;
                                else lastChild              = removeThis->prev ;
  if (  removeThis->prev           ) removeThis->prev->next = removeThis->next ;
                                else firstChild             = removeThis->next ;
  delete removeThis;
  return true;
}

const N::XmlNode * N::XmlNode::FirstChild(const char * _value) const
{
  const XmlNode * node                               ;
  for ( node = firstChild; node; node = node->next ) {
    if (strcmp(node->Value(),_value)==0) return node ;
  }                                                  ;
  return NULL                                        ;
}

const N::XmlNode * N::XmlNode::LastChild(const char * _value) const
{
  const XmlNode * node                                ;
  for ( node = lastChild ; node ; node = node->prev ) {
    if (strcmp(node->Value(),_value)==0) return node  ;
  }                                                   ;
  return NULL                                         ;
}

const N::XmlNode * N::XmlNode::IterateChildren(const XmlNode * previous) const
{
  if ( !previous ) return FirstChild() ;
  return previous->NextSibling()       ;
}

const N::XmlNode * N::XmlNode::IterateChildren(const char * val,const XmlNode * previous) const
{
  if ( !previous ) return FirstChild(val) ;
  return previous->NextSibling      (val) ;
}

const N::XmlNode * N::XmlNode::NextSibling(const char * _value) const
{
  const XmlNode * node                               ;
  for ( node = next; node; node = node->next )       {
    if (strcmp(node->Value(),_value)==0) return node ;
  }                                                  ;
  return NULL                                        ;
}

const N::XmlNode * N::XmlNode::PreviousSibling(const char * _value) const
{
  const XmlNode * node                               ;
  for ( node = prev; node; node = node->prev )       {
    if (strcmp(node->Value(),_value)==0) return node ;
  }                                                  ;
  return NULL                                        ;
}

const N::XmlElement * N::XmlNode::FirstChildElement(void) const
{
  const XmlNode * node                                  ;
  for (node=FirstChild();node;node=node->NextSibling()) {
    if (node->ToElement()) return node->ToElement()     ;
  }                                                     ;
  return NULL                                           ;
}

const N::XmlElement * N::XmlNode::FirstChildElement(const char * _value) const
{
  const XmlNode * node                                             ;
  for (node=FirstChild(_value);node;node=node->NextSibling(_value)) {
    if (node->ToElement()) return node->ToElement()                 ;
  }                                                                 ;
  return NULL                                                       ;
}

const N::XmlElement * N::XmlNode::NextSiblingElement(void) const
{
  const XmlNode * node                                   ;
  for (node=NextSibling();node;node=node->NextSibling()) {
    if (node->ToElement()) return node->ToElement()      ;
  }                                                      ;
  return NULL                                            ;
}

const N::XmlElement * N::XmlNode::NextSiblingElement(const char * _value) const
{
  const XmlNode * node                                               ;
  for (node=NextSibling(_value);node;node=node->NextSibling(_value)) {
    if (node->ToElement()) return node->ToElement()                  ;
  }                                                                  ;
  return NULL                                                        ;
}

const N::XmlDocument * N::XmlNode::GetDocument(void) const
{
  const XmlNode * node                   ;
  for (node=this;node;node=node->parent) {
    if (node->ToDocument())              {
      return node->ToDocument()          ;
    }                                    ;
  }                                      ;
  return NULL                            ;
}

N::XmlNode * N::XmlNode::Identify(const char * p,XML::Encoding encoding)
{
  XmlNode * returnNode = NULL                               ;
  ///////////////////////////////////////////////////////////
  p = SkipWhiteSpace( p, encoding )                         ;
  if ( empty ( p ) ) return NULL                            ;
  if ( *p != '<'   ) return NULL                            ;
  ///////////////////////////////////////////////////////////
  p = SkipWhiteSpace( p, encoding )                         ;
  if ( empty ( p ) ) return NULL                            ;
  ///////////////////////////////////////////////////////////
  const char * xmlHeader     = { "<?xml"     }              ;
  const char * commentHeader = { "<!--"      }              ;
  const char * dtdHeader     = { "<!"        }              ;
  const char * cdataHeader   = { "<![CDATA[" }              ;
  ///////////////////////////////////////////////////////////
  if ( StringEqual( p, xmlHeader     , true  , encoding ) ) {
    returnNode = new XmlDeclaration (    )                  ;
  } else
  if ( StringEqual( p, commentHeader , false , encoding ) ) {
    returnNode = new XmlComment     (    )                  ;
  } else
  if ( StringEqual( p, cdataHeader   , false , encoding ) ) {
    XmlText * text = new XmlText   ( "" )                   ;
    text->SetCDATA( true )                                  ;
    returnNode = text                                       ;
  } else
  if ( StringEqual( p, dtdHeader     , false , encoding ) ) {
    returnNode = new XmlUnknown     (    )                  ;
  } else
  if (       IsAlpha( *(p+1), encoding ) || *(p+1) == '_' ) {
    returnNode = new XmlElement     ( "" )                  ;
  } else                                                    {
    returnNode = new XmlUnknown     (    )                  ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  if ( returnNode ) returnNode->parent = this               ;
  return returnNode                                         ;
}
