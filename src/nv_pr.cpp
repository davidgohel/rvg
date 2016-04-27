#include <Rcpp.h>
#include "nv_pr.h"
#include <iostream>

using namespace Rcpp;

std::string nv_pr::x_tag()
{
  std::stringstream os;
  os << "<xdr:nvSpPr>";
  os << "<xdr:cNvPr id=\"" << this->id << "\" name=\"" << this->label << this->id << "\"/>";
  if( this->editable < 1 ){
    os << "<xdr:cNvSpPr>";
    os << "<a:spLocks noSelect=\"1\" noResize=\"1\" noEditPoints=\"1\" noTextEdit=\"1\" noMove=\"1\" noRot=\"1\" noChangeShapeType=\"1\"/>";
    os << "</xdr:cNvSpPr>" ;
  } else {
    os << "<xdr:cNvSpPr/>";
  }
  os << "<xdr:nvPr/>";
  os << "</xdr:nvSpPr>";
  return os.str();
}

std::string nv_pr::p_tag()
{
  std::stringstream os;
  os << "<p:nvSpPr>";
  os << "<p:cNvPr id=\"" << this->id << "\" name=\"" << this->label << this->id << "\"/>";
    if( this->editable < 1 ){
      os << "<p:cNvSpPr>";
      os << "<a:spLocks noSelect=\"1\" noResize=\"1\" noEditPoints=\"1\" noTextEdit=\"1\" noMove=\"1\" noRot=\"1\" noChangeShapeType=\"1\"/>";
      os << "</p:cNvSpPr>" ;
    } else {
      os << "<p:cNvSpPr/>";
    }
  os << "<p:nvPr/>";
  os << "</p:nvSpPr>";
  return os.str();
}

std::string nv_pr::wps_tag()
{
  std::stringstream os;

  os << "<wps:cNvPr id=\"" << this->id << "\" name=\"" << this->label << this->id << "\"/>";
  if( this->editable < 1 ){
    os << "<wps:cNvSpPr>";
    os << "<a:spLocks noSelect=\"1\" noResize=\"1\" noEditPoints=\"1\" noTextEdit=\"1\" noMove=\"1\" noRot=\"1\" noChangeShapeType=\"1\"/>";
    os << "</wps:cNvSpPr>" ;
  } else {
    os << "<wps:cNvSpPr/>";
  }
  os << "<wps:nvPr/>";

  return os.str();
}

nv_pr::nv_pr(int id, int editable, std::string label ):
  id(id),
  editable(editable), label(label){
}
