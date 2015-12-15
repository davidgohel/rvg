#include <Rcpp.h>

using namespace Rcpp;

class xfrm
{
public:
  xfrm (double, double, double, double, double );
  xfrm (NumericVector, NumericVector );
  std::string xml();

  double x;
  double y;
  double width;
  double height;
  double rot;
};
