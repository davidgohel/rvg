#include <Rcpp.h>
#include <cairo.h>
#include <string.h>


#include <gdtools.h>
// [[Rcpp::depends(gdtools)]]

double str_width(const char *str, int bold, int italic, char *fontface, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::get_font_metrics(str, bold, italic, fontface, fontsize );
  return xx[0];
}
double str_height(const char *str, int bold, int italic, char *fontface, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::get_font_metrics(str, bold, italic, fontface, fontsize );
  return xx[1];
}
double* str_info(const char *str, int bold, int italic, char *fontface, int fontsize ) {
  Rcpp::NumericVector xx = gdtools::get_font_info(str, bold, italic, fontface, fontsize );
  double *out = (double*) malloc(3*sizeof(double));
  out[0] = xx[0];
  out[1] = xx[1];
  out[2] = xx[2];
  return out;
}
