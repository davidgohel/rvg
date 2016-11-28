#include <Rcpp.h>
#include "body_pr.h"
#include <iostream>

using namespace Rcpp;

std::string body_pr::a_tag()
{
  std::stringstream os;

  os << "<a:bodyPr ";
  os << "lIns=\"0\" rIns=\"0\" tIns=\"0\" bIns=\"0\" ";
  os << "anchorCtr=\"1\" anchor=\"ctr\" wrap=\"none\"/>";
  os << "<a:lstStyle/>";

  return os.str();
}

std::string body_pr::wps_tag()
{
  std::stringstream os;

  os << "<wps:bodyPr ";
  os << "lIns=\"0\" rIns=\"0\" tIns=\"0\" bIns=\"0\" ";
  os << "anchor=\"b\" ";
  os << "vert=\"horz\" wrap=\"none\">";
  os << "<a:spAutoFit/>";
  os << "</wps:bodyPr>";

  return os.str();
}
