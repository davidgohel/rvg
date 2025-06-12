#include <Rcpp.h>
using namespace Rcpp;


class clipper
{
public:
  clipper();

  void set_data(NumericVector, NumericVector);
  void set_clipping_region(double, double, double, double);

  void clip_polygon();
  void clip_polyline();

  NumericVector get_x();
  NumericVector get_y();
  IntegerVector get_inside();

  std::vector<NumericVector> get_x_lines();
  std::vector<NumericVector> get_y_lines();

  bool is_point_outside(double point_x, double point_y);


private:
  NumericVector x;
  NumericVector y;
  std::vector<NumericVector> x_lines;
  std::vector<NumericVector> y_lines;

  double clip_left;
  double clip_right;
  double clip_top;
  double clip_bottom;

  std::vector<double> dout_, xout_, yout_;
  std::vector<int> Lout_, iout_;

  bool is_inside(int);
  void intersect();
  void correct();
  void drop_outsiders();
  void drop_neighbors();
  void drop_pairneg();
  bool has_pairneg();

  void add_intersect();
  void group_lines();
  void extract_lines();

};
