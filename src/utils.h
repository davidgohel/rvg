#include <Rcpp.h>

using namespace Rcpp;

double translate_rotate_x(double x, double y, double rot, double h, double w, double hadj);
double translate_rotate_y(double x, double y, double rot, double h, double w, double hadj);
std::string a_path(NumericVector x, NumericVector y, bool close);
