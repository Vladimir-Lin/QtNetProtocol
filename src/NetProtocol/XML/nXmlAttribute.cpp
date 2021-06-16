#include <netprotocol.h>

N::XmlAttribute:: XmlAttribute (void)
                : XmlBase      (    )
{
  document = NULL ;
  prev     = NULL ;
  next     = NULL ;
}

N::XmlAttribute::XmlAttribute(const char * _name,const char * _value)
{
  name     = _name  ;
  value    = _value ;
  document = NULL   ;
  prev     = NULL   ;
  next     = NULL   ;
}

N::XmlAttribute::~XmlAttribute(void)
{
}

const N::XmlAttribute * N::XmlAttribute::Next(void) const
{
  if (next->value.empty() && next->name.empty()) return NULL ;
  return next                                                ;
}

const N::XmlAttribute * N::XmlAttribute::Previous(void) const
{
  if (prev->value.empty() && prev->name.empty()) return NULL ;
  return prev                                                ;
}

int N::XmlAttribute::QueryIntValue(int * ival) const
{
  if (sscanf(value.c_str(), "%d",ival)==1) return XML::Success ;
  return XML::WrongType ;
}

int N::XmlAttribute::QueryDoubleValue(double * dval) const
{
  if (sscanf(value.c_str(),"%lf",dval)==1) return XML::Success ;
  return XML::WrongType ;
}

void N::XmlAttribute::SetIntValue(int _value)
{
  char      buf [64]         ;
  sprintf  (buf,"%d",_value) ;
  SetValue (buf            ) ;
}

void N::XmlAttribute::SetDoubleValue(double _value)
{
  char      buf [256]        ;
  sprintf  (buf,"%g",_value) ;
  SetValue (buf            ) ;
}

int N::XmlAttribute::IntValue(void) const
{
  return atoi ( value . c_str ( ) ) ;
}

double N::XmlAttribute::DoubleValue(void) const
{
  return atof ( value . c_str ( ) ) ;
}

const char * N::XmlAttribute::Name(void) const
{
  return name.c_str();
}

const char * N::XmlAttribute::Value(void) const
{
  return value.c_str();
}

const std::string & N::XmlAttribute::NameTStr(void) const
{
  return name;
}

void N::XmlAttribute::SetName(const char * _name)
{
  name = _name;
}

void N::XmlAttribute::SetValue(const char * _value)
{
  value = _value;
}

N::XmlAttribute * N::XmlAttribute::Next(void)
{
  return const_cast<XmlAttribute*>((const_cast<const XmlAttribute*>(this))->Next()) ;
}

N::XmlAttribute * N::XmlAttribute::Previous(void)
{
  return const_cast<XmlAttribute*>((const_cast<const XmlAttribute*>(this))->Previous()) ;
}

bool N::XmlAttribute::operator == (const XmlAttribute & rhs) const
{
  return rhs.name == name;
}

bool N::XmlAttribute::operator  < (const XmlAttribute & rhs) const
{
  return name < rhs.name;
}

bool N::XmlAttribute::operator  > (const XmlAttribute & rhs) const
{
  return name > rhs.name;
}

void N::XmlAttribute::SetDocument(XmlDocument * doc)
{
  document = doc;
}

const char * N::XmlAttribute::Parse(const char* p,XmlParsing * data,XML::Encoding encoding)
{
  p = SkipWhiteSpace ( p , encoding )                ;
  if ( empty ( p ) ) return NULL                     ;
  ////////////////////////////////////////////////////
  if ( data )                                        {
    data->Stamp( p, encoding )                       ;
    location = data->Cursor( )                       ;
  }                                                  ;
  ////////////////////////////////////////////////////
  const char * pErr = p                              ;
  p = ReadName ( p, &name , encoding )               ;
  if ( empty ( p ) )                                 {
    if ( document )                                  {
      document->SetError                             (
        XmlError_Reading_Attributes                  ,
        pErr,data,encoding                         ) ;
    }                                                ;
    return NULL                                      ;
  }                                                  ;
  ////////////////////////////////////////////////////
  p = SkipWhiteSpace( p, encoding )                  ;
  if ( ! empty ( p ) && *p != '=' )                  {
    value = ""                                       ;
    return p                                         ;
  }                                                  ;
  ////////////////////////////////////////////////////
  if ( empty ( p ) || *p != '=' )                    {
    if ( document )                                  {
      document->SetError                             (
        XmlError_Reading_Attributes                  ,
        p,data,encoding                            ) ;
    }                                                ;
    return NULL                                      ;
  }                                                  ;
  ////////////////////////////////////////////////////
  ++p                                                ; // skip '='
  p = SkipWhiteSpace( p, encoding )                  ;
  if ( empty ( p ) )                                 {
    if ( document )                                  {
      document->SetError                             (
        XmlError_Reading_Attributes                  ,
        p,data,encoding                            ) ;
    }                                                ;
    return NULL                                      ;
  }                                                  ;
  ////////////////////////////////////////////////////
  const char * end                                   ;
  const char   SINGLE_QUOTE = '\''                   ;
  const char   DOUBLE_QUOTE = '\"'                   ;
  ////////////////////////////////////////////////////
  if ( *p == SINGLE_QUOTE )                          {
    ++p                                              ;
    end = "\'"                                       ;
    p = ReadText(p,&value,false,end,false,encoding)  ;
  } else
  if ( *p == DOUBLE_QUOTE )                          {
    ++p                                              ;
    end = "\""                                       ;
    p = ReadText(p,&value,false,end,false,encoding)  ;
  } else                                             {
    bool endAttribute = false                        ;
    value = ""                                       ;
    while (!endAttribute)                            {
      if (empty       ( p)) endAttribute = true ; else
      if (IsWhiteSpace(*p)) endAttribute = true ; else
      if ((*p)=='>'       ) endAttribute = true ; else
      if ((*p)=='/' && (*(p+1))=='>')                {
        endAttribute = true                          ;
      } else
      if ( *p==SINGLE_QUOTE || *p==DOUBLE_QUOTE )    {
        if ( document )                              {
          document->SetError                         (
            XmlError_Reading_Attributes              ,
            p,data,encoding                        ) ;
        }                                            ;
        return NULL                                  ;
      } else                                         {
        value += *p                                  ;
        ++p                                          ;
      }                                              ;
    }                                                ;
  }                                                  ;
  return p                                           ;
}
