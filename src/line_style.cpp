#include <Rcpp.h>
#include "line_style.h"
#include "a_color.h"
#include <iostream>
#include "R_ext/GraphicsEngine.h"

using namespace Rcpp;

std::string line_style::a_tag()
{
  a_color col_(this->col);
  if( col_.is_transparent() > 0 ) return "";

  if( this->width < 0.000001 ) return "";
  if( this->type < 0 ) return "";

  std::stringstream os;

  os << "<a:ln w=\""<< (int)(this->width * 72 / 96 * 12700) << "\" ";
  switch (this->end) {
    case GE_ROUND_CAP: //round
      os << "cap=\"rnd\"";
      break;
    case GE_BUTT_CAP: //mitre
      os << "cap=\"flat\"";
      break;
    case GE_SQUARE_CAP: //bevel
      os << "cap=\"sq\"";
      break;
    default:
      os << "cap=\"rnd\"";
    break;
  }
  os << ">";

  os << col_.solid_fill();

  int newlty = this->type;
  double newlwd = this->width;
  int i;

  switch (this->type) {
  case LTY_BLANK:
    break;
  case LTY_SOLID:
    os << "<a:prstDash val=\"solid\"/>";
    break;
  case LTY_DOTTED:
    os << "<a:prstDash val=\"dot\"/>";
    break;
  case LTY_DASHED:
    os << "<a:prstDash val=\"dash\"/>";
    break;
  case LTY_LONGDASH:
    os << "<a:prstDash val=\"lgDash\"/>";
    break;
  default:
    os << "<a:custDash>";
    for(i=0 ; i<8 && newlty&15 ; i++) {
      int lwd = (int)newlwd * newlty;
      lwd = lwd & 15;
      if( i%2 < 1 )
        os << "<a:ds d=\""<< (int)(lwd * 100000) << "\" ";
      else
        os << "sp=\""<< (int)(lwd * 100000) << "\"/>";
      newlty = newlty>>4;
    }
    os << "</a:custDash>";
  break;
  }

  switch (this->join) {
    case GE_ROUND_JOIN: //round
      os << "<a:round/>";
      break;
    case GE_MITRE_JOIN: //mitre
      os << "<a:miter/>";
      break;
    case GE_BEVEL_JOIN: //bevel
      os << "<a:bevel/>";
      break;
    default:
      os << "<a:round/>";
    break;
  }

  os << "</a:ln>";
  return os.str();
}

line_style::line_style (double width, int col, int type, int join, int end):
  width(width),
  col(col),
  type(type),
  join(join),
  end(end){

}
