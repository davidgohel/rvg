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
  if( this->italic ) os << " i=\"1\"";
  if( this->bold ) os << " b=\"1\"";
  os << ">";
  os << col_.solid_fill();
  os << "<a:latin typeface=\"" << this->fontname << "\"/>";
  os << "<a:cs typeface=\"" << this->fontname << "\"/>";
  os << "<a:ea typeface=\"" << this->fontname << "\"/>";

  os << "</a:rPr>";

  return os.str();
}

std::string rpr::w_tag()
{
  a_color col_(this->col);
  if( col_.is_visible() < 1 ) return "";

  std::stringstream os;

  os << "<w:rPr>";
  os << "<w:rFonts";
  os << " w:ascii=\"" << this->fontname << "\"";
  os << " w:hAnsi=\"" << this->fontname << "\"";
  os << " w:cs=\"" << this->fontname << "\"";
  os << "/>";

  if( this->italic ) os << "<w:i/>";
  if( this->bold ) os << "<w:b/>";

  os << "<w:sz w:val=\"";
  os << (int)(this->size*2);
  os << "\"/>";
  os << "<w:szCs w:val=\"";
  os << (int)(this->size*2);
  os << "\"/>";

  os << col_.w_color();
  if (col_.has_alpha() > 0) {
    os << "<w14:textFill>";
    os << col_.solid_fill_w14();
    os << "</w14:textFill>";
  }
  os << "</w:rPr>";
  return os.str();
}

rpr::rpr (double size, int italic, int bold, int col, std::string fontname ):
  size(size),
  italic(italic),
  bold(bold),
  col(col),
  fontname(fontname){

}
