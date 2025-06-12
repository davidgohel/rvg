#include "ooxml_dev.h"
#include "Rcpp.h"
#include "xfrm.h"
#include "line_style.h"
#include "a_color.h"
#include "a_prstgeom.h"
#include "write_nv_pr.h"

std::string pptx_empty_body_text(){
  std::stringstream os;
  os << "<p:txBody>";
  os << "<a:bodyPr/><a:lstStyle/><a:p/>";
  os << "</p:txBody>";
  return os.str();
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


void pptx_do_polyline(NumericVector x, NumericVector y, const pGEcontext gc,
                      pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  int i;

  for(i = 0 ; i < x.size() ; i++ ){
    x[i] += pptx_obj->offx;
    y[i] += pptx_obj->offy;
  }
  xfrm xfrm_(x, y );
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fputs("<p:sp>", pptx_obj->file);
  write_nv_pr_pptx(dd, "pl");
  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fputs( "<a:custGeom><a:avLst/>", pptx_obj->file );
  fputs( "<a:pathLst>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", a_path(x, y, 0 ).c_str());
  fputs( "</a:pathLst>", pptx_obj->file );
  fputs( "</a:custGeom>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s",pptx_empty_body_text().c_str());
  fputs("</p:sp>", pptx_obj->file);
}


void pptx_line(double x1, double y1, double x2, double y2,
                      const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  Rcpp::NumericVector x_(2);
  Rcpp::NumericVector y_(2);
  x_[0] = x1;
  y_[0] = y1;
  x_[1] = x2;
  y_[1] = y2;
  pptx_obj->clp->set_data(x_, y_);
  pptx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = pptx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = pptx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    pptx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }
}


void pptx_polyline(int n, double *x, double *y, const pGEcontext gc,
                          pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(int i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }
  pptx_obj->clp->set_data(x_, y_);
  pptx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = pptx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = pptx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    pptx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }
}

void pptx_polygon(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  int i;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }

  pptx_obj->clp->set_data(x_, y_);
  pptx_obj->clp->clip_polygon();
  //
  Rcpp::NumericVector x__ = pptx_obj->clp->get_x();
  Rcpp::NumericVector y__ = pptx_obj->clp->get_y();
  for(i = 0 ; i < y__.size() ; i++ ){
    x__[i] += pptx_obj->offx;
    y__[i] += pptx_obj->offy;
  }
  xfrm xfrm_(x__, y__);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<p:sp>", pptx_obj->file);
  write_nv_pr_pptx(dd, "pg");
  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fputs("<a:custGeom><a:avLst/>", pptx_obj->file );
  fputs( "<a:pathLst>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", a_path(x__, y__, 1 ).c_str());
  fputs( "</a:pathLst>", pptx_obj->file );
  fputs("</a:custGeom>", pptx_obj->file );
  if( fill_.is_visible() > 0 )
    fprintf(pptx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s",pptx_empty_body_text().c_str());
  fputs("</p:sp>", pptx_obj->file);
}

void pptx_path(double *x, double *y,
                      int npoly, int *nper,
                      Rboolean winding,
                      const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  int index = 0;
  for (int i = 0; i < npoly; i++) {
    Rcpp::NumericVector x_(nper[i]);
    Rcpp::NumericVector y_(nper[i]);

    for(int p = 0 ; p < nper[i] ; p++ ){
      x_[p] = x[index];
      y_[p] = y[index];
      index++;
    }
    pptx_obj->clp->set_data(x_, y_);
    pptx_obj->clp->clip_polyline();

    std::vector<NumericVector> x_array = pptx_obj->clp->get_x_lines();
    std::vector<NumericVector> y_array = pptx_obj->clp->get_y_lines();

    for( size_t l = 0 ; l < x_array.size() ; l++ ){
      double *tempx = x_array.at(l).begin();
      double *tempy = y_array.at(l).begin();
      pptx_polygon(nper[i], tempx, tempy, gc, dd);
    }
  }

}


void pptx_rect(double x0, double y0, double x1, double y1,
                      const pGEcontext gc, pDevDesc dd) {

  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  Rcpp::NumericVector x_(4);
  Rcpp::NumericVector y_(4);
  x_[0] = x0;
  y_[0] = y0;
  x_[1] = x1;
  y_[1] = y0;
  x_[2] = x1;
  y_[2] = y1;
  x_[3] = x0;
  y_[3] = y1;
  pptx_obj->clp->set_data(x_, y_);
  pptx_obj->clp->clip_polygon();
  Rcpp::NumericVector x__ = pptx_obj->clp->get_x();
  Rcpp::NumericVector y__ = pptx_obj->clp->get_y();
  for(int i = 0 ; i < x__.size() ; i++ ){
    x__[i] += pptx_obj->offx;
    y__[i] += pptx_obj->offy;
  }
  xfrm xfrm_(x__, y__);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<p:sp>", pptx_obj->file);
  write_nv_pr_pptx(dd, "rc");
  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  if( fill_.is_visible() > 0 )
    fprintf(pptx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s",pptx_empty_body_text().c_str());
  fputs("</p:sp>", pptx_obj->file);
}

void pptx_circle(double x, double y, double r, const pGEcontext gc,
                        pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  if (pptx_obj->clp->is_point_outside(x, y)) {
    return;
  }
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );
  xfrm xfrm_(pptx_obj->offx + x -r, pptx_obj->offy + y - r, r * 2, r * 2 , 0.0 );


  fputs("<p:sp>", pptx_obj->file);
  write_nv_pr_pptx(dd, "pt");
  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("ellipse").c_str());
  if( fill_.is_visible() > 0 )
    fprintf(pptx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s",pptx_empty_body_text().c_str());

  fputs("</p:sp>", pptx_obj->file);
}



std::string xlsx_empty_body_text(){
  std::stringstream os;
  os << "<xdr:txBody>";
  os << "<a:bodyPr/><a:lstStyle/><a:p/>";
  os << "</xdr:txBody>";
  return os.str();
}


void xlsx_do_polyline(NumericVector x, NumericVector y, const pGEcontext gc,
                      pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;

  int i;

  for(i = 0 ; i < x.size() ; i++ ){
    x[i] += xlsx_obj->offx;
    y[i] += xlsx_obj->offy;
  }
  xfrm xfrm_(x, y );
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fputs("<xdr:sp>", xlsx_obj->file);
  write_nv_pr_xlsx(dd, "pl");
  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fputs( "<a:custGeom><a:avLst/>", xlsx_obj->file );
  fputs( "<a:pathLst>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", a_path(x, y, 0 ).c_str());
  fputs( "</a:pathLst>", xlsx_obj->file );
  fputs( "</a:custGeom>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s",xlsx_empty_body_text().c_str());
  fputs("</xdr:sp>", xlsx_obj->file);
}



void xlsx_line(double x1, double y1, double x2, double y2,
                      const pGEcontext gc, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;

  Rcpp::NumericVector x_(2);
  Rcpp::NumericVector y_(2);
  x_[0] = x1;
  y_[0] = y1;
  x_[1] = x2;
  y_[1] = y2;
  xlsx_obj->clp->set_data(x_, y_);
  xlsx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = xlsx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = xlsx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    xlsx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }
}


void xlsx_polyline(int n, double *x, double *y, const pGEcontext gc,
                          pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(int i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }
  xlsx_obj->clp->set_data(x_, y_);
  xlsx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = xlsx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = xlsx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    xlsx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }
}

void xlsx_polygon(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  int i;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }

  xlsx_obj->clp->set_data(x_, y_);
  xlsx_obj->clp->clip_polygon();
  //
  Rcpp::NumericVector x__ = xlsx_obj->clp->get_x();
  Rcpp::NumericVector y__ = xlsx_obj->clp->get_y();
  for(i = 0 ; i < y__.size() ; i++ ){
    x__[i] += xlsx_obj->offx;
    y__[i] += xlsx_obj->offy;
  }
  xfrm xfrm_(x__, y__);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<xdr:sp>", xlsx_obj->file);
  write_nv_pr_xlsx(dd, "pg");
  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fputs("<a:custGeom><a:avLst/>", xlsx_obj->file );
  fputs( "<a:pathLst>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", a_path(x__, y__, 1 ).c_str());
  fputs( "</a:pathLst>", xlsx_obj->file );
  fputs("</a:custGeom>", xlsx_obj->file );
  if( fill_.is_visible() > 0 )
    fprintf(xlsx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(xlsx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s",xlsx_empty_body_text().c_str());
  fputs("</xdr:sp>", xlsx_obj->file);
}

void xlsx_rect(double x0, double y0, double x1, double y1,
                      const pGEcontext gc, pDevDesc dd) {

  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  Rcpp::NumericVector x_(4);
  Rcpp::NumericVector y_(4);
  x_[0] = x0;
  y_[0] = y0;
  x_[1] = x1;
  y_[1] = y0;
  x_[2] = x1;
  y_[2] = y1;
  x_[3] = x0;
  y_[3] = y1;
  xlsx_obj->clp->set_data(x_, y_);
  xlsx_obj->clp->clip_polygon();
  Rcpp::NumericVector x__ = xlsx_obj->clp->get_x();
  Rcpp::NumericVector y__ = xlsx_obj->clp->get_y();
  for(int i = 0 ; i < x__.size() ; i++ ){
    x__[i] += xlsx_obj->offx;
    y__[i] += xlsx_obj->offy;
  }
  xfrm xfrm_(x__, y__);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<xdr:sp>", xlsx_obj->file);
  write_nv_pr_xlsx(dd, "rc");
  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(xlsx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  if( fill_.is_visible() > 0 )
    fprintf(xlsx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(xlsx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s",xlsx_empty_body_text().c_str());
  fputs("</xdr:sp>", xlsx_obj->file);
}

void xlsx_circle(double x, double y, double r, const pGEcontext gc,
                        pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  if (xlsx_obj->clp->is_point_outside(x, y)) {
    return;
  }
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );
  xfrm xfrm_(xlsx_obj->offx + x -r, xlsx_obj->offy + y - r, r * 2, r * 2 , 0.0 );


  fputs("<xdr:sp>", xlsx_obj->file);
  write_nv_pr_xlsx(dd, "pt");
  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(xlsx_obj->file,"%s", a_prstgeom::a_tag("ellipse").c_str());
  if( fill_.is_visible() > 0 )
    fprintf(xlsx_obj->file, "%s", fill_.solid_fill().c_str());
  fprintf(xlsx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s",xlsx_empty_body_text().c_str());

  fputs("</xdr:sp>", xlsx_obj->file);
}

