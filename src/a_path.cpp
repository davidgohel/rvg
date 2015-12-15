#include <Rcpp.h>
#include "a_path.h"
#include <iostream>

using namespace Rcpp;

std::string a_path::a_tag(NumericVector x, NumericVector y, bool close)
{
  std::stringstream os;
  double maxx = max(x);
  double maxy = max(y);
  double minx = min(x);
  double miny = min(y);

  os << "<a:path ";
  os << "w=\""<< (int)((maxx-minx)*12700) << "\" ";
  os << "h=\""<< (int)((maxy-miny)*12700) << "\">";

  os << "<a:moveTo>";
  os << "<a:pt ";
  os << "x=\"" << (int)((x[0] - minx)*12700) << "\" ";
  os << "y=\"" <<  (int)((y[0] - miny)*12700) << "\"/>";
  os << "</a:moveTo>";

  for ( int i = 1; i < x.size(); i++) {
    os << "<a:lnTo>";
    os << "<a:pt x=\"" << (int)((x[i] - minx)*12700) << "\" y=\"" <<  (int)((y[i] - miny)*12700) << "\"/>";
    os << "</a:lnTo>";
  }

  if( close > 0 )
    os << "<a:close/>";

  os << "</a:path>";

  return os.str();
}
