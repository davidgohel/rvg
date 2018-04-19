#include <math.h>
#include <iostream>
#include <Rcpp.h>

using namespace Rcpp;

double translate_rotate_x(double x, double y, double rot, double h, double w, double hadj) {
  double pi = 3.141592653589793115997963468544185161590576171875;
  double alpha = -rot * pi / 180;

  double Px = x + (0.5-hadj) * w;
  double Py = y - 0.5 * h;

  double _cos = cos( alpha );
  double _sin = sin( alpha );

  double Ppx = x + (Px-x) * _cos - (Py-y) * _sin ;

  return Ppx - 0.5 * w;
}


double translate_rotate_y(double x, double y, double rot, double h, double w, double hadj) {
  double pi = 3.141592653589793115997963468544185161590576171875;
  double alpha = -rot * pi / 180;

  double Px = x + (0.5-hadj) * w;
  double Py = y - 0.5 * h;

  double _cos = cos( alpha );
  double _sin = sin( alpha );

  double Ppy = y + (Px-x) * _sin + (Py-y) * _cos;

  return Ppy - 0.5 * h;
}

std::string a_path(NumericVector x, NumericVector y, bool close)
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
