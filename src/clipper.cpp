#include <Rcpp.h>
#include "clipper.h"
using namespace Rcpp;
#include <math.h>

struct pg_coord {
  pg_coord(double d, double x, double y, int L) : d(d), x(x), y(y), L(L)  { }
  double d;
  double x;
  double y;
  int L;
};


bool operator<(const pg_coord &lhs, const pg_coord &rhs) {
  return lhs.d < rhs.d;
}

struct ln_coord {
  ln_coord(double d, double x, double y, int L, int inside) : d(d), x(x), y(y), L(L), inside(inside)  { }
  double d;
  double x;
  double y;
  int L;
  int inside;
};

bool operator<(const ln_coord &lhs, const ln_coord &rhs) {
  return lhs.d < rhs.d;
}


void clipper::set_data(NumericVector x_, NumericVector y_)
{
  x = x_;
  y = y_;
}

void clipper::set_clipping_region(double clip_left_, double clip_bottom_,
                                  double clip_right_, double clip_top_){
  clip_left = clip_left_;
  clip_right = clip_right_;
  clip_bottom = clip_bottom_;
  clip_top = clip_top_;
}

void clipper::clip_polygon(){

  xout_.clear();
  yout_.clear();
  Lout_.clear();
  dout_.clear();

  intersect();
  while( has_pairneg() > 0 )
    drop_pairneg();

  correct();
  drop_outsiders();
  drop_neighbors();


  NumericVector x_ = wrap(xout_);
  NumericVector y_ = wrap(yout_);
}

void clipper::clip_polyline(){

  xout_.clear();
  yout_.clear();
  Lout_.clear();
  dout_.clear();
  iout_.clear();

  add_intersect();
  group_lines();
  extract_lines();
}


bool clipper::is_inside(int i){
  return x[i] >= clip_left &&
    x[i] <= clip_right &&
    y[i] >= clip_bottom &&
    y[i] <= clip_top;
}

NumericVector clipper::get_x(){
  return wrap(xout_);
}
NumericVector clipper::get_y(){
  return wrap(yout_);
}
IntegerVector clipper::get_inside(){
  return wrap(iout_);
}


void clipper::intersect(){

  double D, x1, x2, y1, y2;
  double cx, cy;
  double dist_;
  double curr_x, curr_y;

  for( int i = 0 ; i < x.size() -1 ; i++ ){
    std::vector<pg_coord> out;
    x1 = x[i];
    x2 = x[i+1];
    y1 = y[i];
    y2 = y[i+1];
    if(i == 0 && is_inside(0) ){
      out.push_back(pg_coord(0, x1, y1, 0));
    }

    D = pow(x1 - x2, 2) + pow(y1 - y2, 2);
    D = pow( D, .5);
    cx = (x2 - x1) / D; // Cos directeur sur x
    cy = (y2 - y1) / D; // Cos directeur sur y

    // distance de S1 a ymin
    // Le point d'intersection est x ymin
    dist_ = (clip_bottom - y1 ) / cy;
    curr_x = x1 + cx * dist_;
    if(dist_ > 0 && dist_ <= D){
      if(curr_x >= clip_left && curr_x <= clip_right) {
        out.push_back(pg_coord(dist_, curr_x, clip_bottom, 1));
      } else if(curr_x < clip_left) {
        out.push_back(pg_coord(dist_, curr_x, clip_bottom, -2));
      } else if(curr_x > clip_right ) {
        out.push_back(pg_coord(dist_, curr_x, clip_bottom, -7));
      }
    }

    // distance de S1 a clip_left
    // Le point d'intersection est clip_left y
    dist_ = (clip_left - x1)/cx;
    curr_y = y1 + cy*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_y >= clip_bottom && curr_y <= clip_top) {
        out.push_back(pg_coord(dist_, clip_left, curr_y, 2));
      } else if(curr_y < clip_bottom) {
        out.push_back(pg_coord(dist_, clip_left, curr_y, -1));
      } else if(curr_y > clip_top) {
        out.push_back(pg_coord(dist_, clip_left, curr_y, -4));
      }
    }



    // distance de S1 a clip_top
    // Le point d'intersection est curr_x clip_top
    dist_ = (clip_top - y1)/cy;
    curr_x = x1 + cx*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_x >= clip_left && curr_x <= clip_right) {
        out.push_back(pg_coord(dist_, curr_x, clip_top, 3));
      } else if(curr_x < clip_left) {
        out.push_back(pg_coord(dist_, curr_x, clip_top, -3));
      } else if(curr_x > clip_right ) {
        out.push_back(pg_coord(dist_, curr_x, clip_top,-6));
      }

    }

    // distance de S1 a clip_right
    // Le point d'intersection est clip_right curr_y
    dist_ = (clip_right - x1)/cx;
    curr_y = y1 + cy*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_y >= clip_bottom && curr_y <= clip_top) {
        out.push_back(pg_coord(dist_, clip_right, curr_y, 4));
      } else if(curr_y < clip_bottom) {
        out.push_back(pg_coord(dist_, clip_right, curr_y, -8));
      } else if(curr_y > clip_top) {
        out.push_back(pg_coord(dist_, clip_right, curr_y, -5));
      }
    }


    std::stable_sort(out.begin(), out.end());
    for( size_t o = 0 ; o < out.size() ; o ++){
      dout_.push_back(out.at(o).d);
      xout_.push_back(out.at(o).x);
      yout_.push_back(out.at(o).y);
      Lout_.push_back(out.at(o).L);

    }
    if( is_inside(i+1)) {
      dout_.push_back(0);
      xout_.push_back(x2);
      yout_.push_back(y2);
      Lout_.push_back(0);
    }
  }
}

void clipper::add_intersect(){

  double D, x1, x2, y1, y2;
  double cx, cy;
  double dist_;
  double curr_x, curr_y;

  for( int i = 0 ; i < x.size() -1 ; i++ ){
    std::vector<ln_coord> out;
    x1 = x[i];
    x2 = x[i+1];
    y1 = y[i];
    y2 = y[i+1];
    if(i == 0 ){
      out.push_back(ln_coord(0, x1, y1, 0, is_inside(i)));
    }

    D = pow(x1 - x2, 2) + pow(y1 - y2, 2);
    D = pow( D, .5);
    cx = (x2 - x1) / D; // Cos directeur sur x
    cy = (y2 - y1) / D; // Cos directeur sur y

    // distance de S1 a ymin
    // Le point d'intersection est x ymin
    dist_ = (clip_bottom - y1 ) / cy;
    curr_x = x1 + cx * dist_;
    if(dist_ > 0 && dist_ <= D){
      if(curr_x >= clip_left && curr_x <= clip_right) {
        out.push_back(ln_coord(dist_, curr_x, clip_bottom, 1, 1));
      } else if(curr_x < clip_left) {
        out.push_back(ln_coord(dist_, curr_x, clip_bottom, -2, 0));
      } else if(curr_x > clip_right ) {
        out.push_back(ln_coord(dist_, curr_x, clip_bottom, -7, 0));
      }
    }

    // distance de S1 a clip_left
    // Le point d'intersection est clip_left y
    dist_ = (clip_left - x1)/cx;
    curr_y = y1 + cy*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_y >= clip_bottom && curr_y <= clip_top) {
        out.push_back(ln_coord(dist_, clip_left, curr_y, 2, 1));
      } else if(curr_y < clip_bottom) {
        out.push_back(ln_coord(dist_, clip_left, curr_y, -1, 0));
      } else if(curr_y > clip_top) {
        out.push_back(ln_coord(dist_, clip_left, curr_y, -4, 0));
      }
    }



    // distance de S1 a clip_top
    // Le point d'intersection est curr_x clip_top
    dist_ = (clip_top - y1)/cy;
    curr_x = x1 + cx*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_x >= clip_left && curr_x <= clip_right) {
        out.push_back(ln_coord(dist_, curr_x, clip_top, 3, 1));
      } else if(curr_x < clip_left) {
        out.push_back(ln_coord(dist_, curr_x, clip_top, -3, 0));
      } else if(curr_x > clip_right ) {
        out.push_back(ln_coord(dist_, curr_x, clip_top,-6, 0));
      }

    }

    // distance de S1 a clip_right
    // Le point d'intersection est clip_right curr_y
    dist_ = (clip_right - x1)/cx;
    curr_y = y1 + cy*dist_;
    if(dist_ > 0 && dist_ <=D){
      if(curr_y >= clip_bottom && curr_y <= clip_top) {
        out.push_back(ln_coord(dist_, clip_right, curr_y, 4, 1));
      } else if(curr_y < clip_bottom) {
        out.push_back(ln_coord(dist_, clip_right, curr_y, -8, 0));
      } else if(curr_y > clip_top) {
        out.push_back(ln_coord(dist_, clip_right, curr_y, -5, 0));
      }
    }


    std::stable_sort(out.begin(), out.end());
    for( size_t o = 0 ; o < out.size() ; o ++){
      dout_.push_back(out.at(o).d);
      xout_.push_back(out.at(o).x);
      yout_.push_back(out.at(o).y);
      Lout_.push_back(out.at(o).L);
      iout_.push_back(out.at(o).inside);
    }
    dout_.push_back(0);
    xout_.push_back(x2);
    yout_.push_back(y2);
    Lout_.push_back(0);
    iout_.push_back(is_inside(i+1));
  }
}


void clipper::correct(){

  if( dout_.size() < 1) return;

  dout_.push_back(dout_.at(0));
  xout_.push_back(xout_.at(0));
  yout_.push_back(yout_.at(0));
  Lout_.push_back(Lout_.at(0));

  std::map< int, int > alter_id_;
  alter_id_[-1] = -2;
  alter_id_[-2] = -1;
  alter_id_[-3] = -4;
  alter_id_[-4] = -3;
  alter_id_[-5] = -6;
  alter_id_[-6] = -5;
  alter_id_[-7] = -8;
  alter_id_[-8] = -7;

  std::map< int, double > newx_;
  newx_[-1] = clip_left;
  newx_[-2] = clip_left;
  newx_[-3] = clip_left;
  newx_[-4] = clip_left;
  newx_[-5] = clip_right;
  newx_[-6] = clip_right;
  newx_[-7] = clip_right;
  newx_[-8] = clip_right;

  std::map< int, double > newy_;
  newy_[-1] = clip_bottom;
  newy_[-2] = clip_bottom;
  newy_[-3] = clip_top;
  newy_[-4] = clip_top;
  newy_[-5] = clip_top;
  newy_[-6] = clip_top;
  newy_[-7] = clip_bottom;
  newy_[-8] = clip_bottom;

  int L1, L2;

  for( size_t i = 0 ; i < dout_.size() -1 ; i++ ){
    L1 = Lout_.at(i);
    L2 = Lout_.at(i+1);
    if( L1 < 0 ){
      if( L2 == alter_id_.at(L1) ){
        xout_.at(i) = newx_.at(L1);
        yout_.at(i) = newy_.at(L1);
        dout_.at(i) = 0.0;
        Lout_.at(i) = 0;
        xout_.at(i+1) = newx_.at(L1);
        yout_.at(i+1) = newy_.at(L1);
        dout_.at(i+1) = 0.0;
        Lout_.at(i+1) = 0;
      }
    }
  }

}

bool clipper::has_pairneg(){
  for( size_t i = 0 ; i < dout_.size() - 1; i++ ){
    if( Lout_.at(i) < 0 && Lout_.at(i)==Lout_.at(i+1) ){
      return 1;
    }
  }
  return 0;
}

void clipper::drop_pairneg(){
  if( dout_.size() < 1) return;
  std::vector<double> dout__, xout__, yout__;
  std::vector<int> Lout__;
  int last = dout_.size() - 1;
  int i = 0;
  for( i = 0 ; i < last; i++ ){
    if( Lout_.at(i) < 0 && Lout_.at(i)==Lout_.at(i+1) ){
      i++;
    } else {
      xout__.push_back(xout_.at(i));
      yout__.push_back(yout_.at(i));
      dout__.push_back(dout_.at(i));
      Lout__.push_back(Lout_.at(i));
    }

  }
  if( i == last ){
    xout__.push_back(xout_.at(last));
    yout__.push_back(yout_.at(last));
    dout__.push_back(dout_.at(last));
    Lout__.push_back(Lout_.at(last));
  }
  xout_ = xout__;
  yout_ = yout__;
  Lout_ = Lout__;
  dout_ = dout__;
}

void clipper::drop_outsiders(){

  if( dout_.size() < 1) return;
  std::vector<double> dout__, xout__, yout__;
  std::vector<int> Lout__;

  for( size_t i = 0 ; i < dout_.size(); i++ ){
    if( Lout_.at(i) > -1 ){
      xout__.push_back(xout_.at(i));
      yout__.push_back(yout_.at(i));
      dout__.push_back(dout_.at(i));
      Lout__.push_back(Lout_.at(i));
    }
  }
  xout_ = xout__;
  yout_ = yout__;
  Lout_ = Lout__;
  dout_ = dout__;
}


void clipper::group_lines(){


  std::vector<double> dout__, xout__, yout__;
  std::vector<int> Lout__, iout__;

  int first_ = -1;
  int id = 0;
  size_t i = 0;

  if( dout_.size() < 1) return ;

  for( i = 0 ; i < dout_.size(); i++ ){
    if( iout_.at(i) > 0 ){
      first_ = i;
      break;
    }
  }

  if( first_ < 0 ) {
    xout_.clear();
    yout_.clear();
    Lout_.clear();
    dout_.clear();
    iout_.clear();
    return;
  }

  i = first_;

  while( i < dout_.size()){

    if( iout_.at(i) < 1 && x.size() > 0 ){
      id++;
    } else if( iout_.at(i) > 0 ){
      xout__.push_back(xout_.at(i));
      yout__.push_back(yout_.at(i));
      dout__.push_back(dout_.at(i));
      Lout__.push_back(Lout_.at(i));
      iout__.push_back(id);
    }

    i++;

  }

  xout_ = xout__;
  yout_ = yout__;
  Lout_ = Lout__;
  dout_ = dout__;
  iout_ = iout__;

}

void clipper::extract_lines(){
  x_lines.clear();
  y_lines.clear();

  if( dout_.size() < 1) return;

  int first_id = iout_.at(0);
  std::vector<double> x_, y_;

  for( size_t i = 0 ; i < dout_.size(); i++ ){
    if( first_id != iout_.at(i) ){
      x_lines.push_back(wrap(x_));
      y_lines.push_back(wrap(y_));
      x_.clear();
      y_.clear();
      first_id = iout_.at(i);
    }
    x_.push_back(xout_.at(i));
    y_.push_back(yout_.at(i));
  }

  x_lines.push_back(wrap(x_));
  y_lines.push_back(wrap(y_));

}

double clipper::get_xmin(){
  double xmin = xout_.at(0);
  for(int i = 1 ; xout_.size() ; i++ ){
    if( xout_.at(i) < xmin ) xmin = xout_.at(i);
  }
  return xmin;
}

double clipper::get_ymin(){
  double ymin = yout_.at(0);
  for(int i = 1 ; yout_.size() ; i++ ){
    if( yout_.at(i) < ymin ) ymin = yout_.at(i);
  }
  return ymin;
}
double clipper::get_xmax(){
  double xmax = xout_.at(0);
  for(int i = 1 ; xout_.size() ; i++ ){
    if( xout_.at(i) > xmax ) xmax = xout_.at(i);
  }
  return xmax;
}
double clipper::get_ymax(){
  double ymax = yout_.at(0);
  for(int i = 1 ; yout_.size() ; i++ ){
    if( yout_.at(i) > ymax ) ymax = yout_.at(i);
  }
  return ymax;
}

std::vector<NumericVector> clipper::get_x_lines(){
  return x_lines;
}

std::vector<NumericVector> clipper::get_y_lines(){
  return y_lines;
}

void clipper::drop_neighbors(){

  if( dout_.size() < 1) return;

  std::vector<double> dout__, xout__, yout__;
  std::vector<int> Lout__;
  double diffx_, diffy_;
  int size_ = dout_.size();

  for( int i = 0 ; i < size_ - 1; i++ ){
    diffx_ = std::abs( xout_.at(i) - xout_.at(i + 1) );
    diffy_ = std::abs( yout_.at(i) - yout_.at(i + 1) );
    if( diffx_ > .01 || diffy_ > .01){
      xout__.push_back(xout_.at(i));
      yout__.push_back(yout_.at(i));
      dout__.push_back(dout_.at(i));
      Lout__.push_back(Lout_.at(i));
    }
  }
  diffx_ = std::abs( xout_.at(size_-1) - xout_.at(0) );
  diffy_ = std::abs( yout_.at(size_-1) - yout_.at(0) );
  if( diffx_ > .01 || diffy_ > .01){
    xout__.push_back(xout_.at(size_-1));
    yout__.push_back(yout_.at(size_-1));
    dout__.push_back(dout_.at(size_-1));
    Lout__.push_back(Lout_.at(size_-1));
  }
  xout_ = xout__;
  yout_ = yout__;
  Lout_ = Lout__;
  dout_ = dout__;
}

clipper::clipper (){
}
