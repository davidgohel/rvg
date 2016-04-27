#include <Rcpp.h>
#include "empty_body_text.h"
#include <iostream>

using namespace Rcpp;

std::string empty_body_text::p_tag()
{
  std::stringstream os;
  os << "<p:txBody>";
  os << "<a:bodyPr/><a:lstStyle/><a:p/>";
  os << "</p:txBody>";
  return os.str();
}

std::string empty_body_text::x_tag()
{
  std::stringstream os;
  os << "<xdr:txBody>";
  os << "<a:bodyPr/><a:lstStyle/><a:p/>";
  os << "</xdr:txBody>";
  return os.str();
}
std::string empty_body_text::wps_tag()
{
  return "<wps:bodyPr/>";
}
