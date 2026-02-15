#include <Rcpp.h>
#include "rpr.h"
#include "a_color.h"
#include <iostream>

using namespace Rcpp;

std::string rpr::a_tag()
{
  a_color col_(this->col);
  if( col_.is_visible() < 1 ) return "";

  std::stringstream os;

  os << "<a:rPr sz=\"";
  os << (int)(this->size*100);
  os << "\"";
  os << " i=\"" << (this->italic ? "1" : "0") << "\"";
  os << " b=\"" << (this->bold ? "1" : "0") << "\"";
  os << ">";
  os << col_.solid_fill();
  os << "<a:latin typeface=\"" << this->fontname << "\"/>";
  os << "<a:cs typeface=\"" << this->fontname << "\"/>";

  os << "</a:rPr>";

  return os.str();
}

rpr::rpr (double size, int italic, int bold, int col, std::string fontname ):
  size(size),
  italic(italic),
  bold(bold),
  col(col),
  fontname(fontname){

}
