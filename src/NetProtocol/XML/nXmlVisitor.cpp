#include <netprotocol.h>

N::XmlVisitor:: XmlVisitor(void)
{
}

N::XmlVisitor::~XmlVisitor(void)
{
}

bool N::XmlVisitor::VisitEnter(const XmlDocument & doc)
{
  return true;
}

bool N::XmlVisitor::VisitExit(const XmlDocument & doc)
{
  return true;
}

bool N::XmlVisitor::VisitEnter(const XmlElement & element,const XmlAttribute * firstAttribute)
{
  return true;
}

bool N::XmlVisitor::VisitExit(const XmlElement & element)
{
  return true;
}

bool N::XmlVisitor::Visit(const XmlDeclaration & declaration)
{
  return true;
}

bool N::XmlVisitor::Visit(const XmlText & text)
{
  return true;
}

bool N::XmlVisitor::Visit(const XmlComment & comment)
{
  return true;
}

bool N::XmlVisitor::Visit(const XmlUnknown & unknown)
{
  return true;
}
