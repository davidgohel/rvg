#include <Rcpp.h>
#include "a_color.h"
#include <iostream>
#include "R_ext/GraphicsDevice.h"
using namespace Rcpp;

std::string a_color::solid_fill()
{

  char col_buf[ 100 ];
  snprintf(col_buf, sizeof(col_buf), "%02X%02X%02X", R_RED(this->col), R_GREEN(this->col), R_BLUE(this->col));
  std::string col_str = col_buf;

  std::stringstream os;
  os << "<a:solidFill><a:srgbClr val=\"";
  os << col_str;
  os << "\">";
  os << "<a:alpha val=\"" << (int)(this->alpha / 255.0 * 100000) << "\"/>";
  os << "</a:srgbClr></a:solidFill>";
  return os.str();
}

int a_color::is_visible() {
  return (this->col != NA_INTEGER) && (this->alpha != 0);
}

int a_color::is_transparent() {
  return (R_ALPHA(this->col) == 0);
}

a_color::a_color (int col ):
  col(col){
  this->alpha = R_ALPHA(col);

}
