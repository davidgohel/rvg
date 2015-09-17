#include <Rcpp.h>


#include <gdtools.h>
// [[Rcpp::depends(gdtools)]]

double str_width(const char *str, int bold, int italic, char *fontname, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::str_extents(str, fontname, fontsize, bold, italic );
  return xx[0];
}
double str_height(const char *str, int bold, int italic, char *fontname, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::str_extents(str, fontname, fontsize, bold, italic );
  return xx[1];
}
double* str_info(const char *str, int bold, int italic, char *fontname, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::str_metrics(str, fontname, fontsize, bold, italic );
  double *out = (double*) malloc(3*sizeof(double));
  out[0] = xx[0];//width
  out[1] = xx[1];//ascent
  out[2] = xx[2];//descent
  return out;
}
