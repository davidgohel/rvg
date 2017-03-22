/*
 * This file is part of rvg.
 * Copyright (c) 2015, David Gohel All rights reserved.
 *
 * rvg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rvg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rvg. If not, see <http://www.gnu.org/licenses/>.
**/

#include "Rcpp.h"
#include <gdtools.h>
#include <string.h>
#include "R_ext/GraphicsEngine.h"
#include "rotate.h"
#include "fonts.h"
#include "xfrm.h"
#include "nv_pr.h"
#include "a_color.h"
#include "rpr.h"
#include "ppr.h"
#include "line_style.h"
#include "main_tree.h"
#include "body_pr.h"
#include "a_path.h"
#include "a_prstgeom.h"
#include "empty_body_text.h"
#include "clipper.h"

// SVG device metadata
class PPTX_dev {
public:
  FILE *file;
  std::string filename;
  int pageno;
  int id;
  double clipleft, clipright, cliptop, clipbottom;
  double offx;
  double offy;

  std::string raster_prefix;
  int img_last_id;

  Rcpp::List system_aliases;
  Rcpp::List user_aliases;

  bool editable;
  int standalone;
  XPtrCairoContext cc;
  clipper *clp;

  PPTX_dev(std::string filename_,
           Rcpp::List& aliases_,
           bool editable_, double offx_, double offy_ , int id_,
           std::string raster_prefix_,
           int rel_last_id_, int standalone_,
           double width_, double height_ ):
      filename(filename_),
      pageno(0),
	    id(id_),
	    offx(offx_), offy(offy_),
	    raster_prefix(raster_prefix_), img_last_id(rel_last_id_),
	    system_aliases(Rcpp::wrap(aliases_["system"])),
	    user_aliases(Rcpp::wrap(aliases_["user"])),
	    editable(editable_),
	    standalone(standalone_),
      cc(gdtools::context_create()){

    file = fopen(R_ExpandFileName(filename.c_str()), "w");
    clipleft = 0.0;
    clipright = width_;
    cliptop = 0.0;
    clipbottom = height_;
    clp = new clipper();
  }

  bool ok() const {
    return file != NULL;
  }

  int new_id() {
  	id++;
  	return id;
  }
  int nex_id_rel() {
    img_last_id++;
    return img_last_id;
  }
  ~PPTX_dev() {
    if (ok())
      fclose(file);
  }
};

void write_nv_pr_pptx(pDevDesc dev, const char *label) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dev->deviceSpecific;
  int idx = pptx_obj->new_id();
  nv_pr obj(idx, (int)(pptx_obj->editable), label);
  fprintf(pptx_obj->file, "%s", obj.p_tag().c_str());
}


void write_t_pptx(pDevDesc dev, const char* text) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dev->deviceSpecific;
  fputs("<a:t>", pptx_obj->file );

  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", pptx_obj->file); break;
    case '<': fputs("&lt;",  pptx_obj->file); break;
    case '>': fputs("&gt;",  pptx_obj->file); break;
    default:  fputc(*cur,    pptx_obj->file);
    }
  }
  fputs("</a:t>", pptx_obj->file );
}


void write_text_body_pptx(pDevDesc dd, R_GE_gcontext *gc, const char* text, double hadj, double fontsize, double fontheight) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dd->deviceSpecific;
  ppr a_ppr_(hadj, fontsize);
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases, pptx_obj->user_aliases);

  rpr rpr_(fontsize, is_italic(gc->fontface), is_bold(gc->fontface), gc->col, fontname_);

  fputs("<p:txBody>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", body_pr::a_tag().c_str() );
  fputs("<a:p>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", a_ppr_.a_tag().c_str() );
  fputs("<a:r>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", rpr_.a_tag().c_str() );
  write_t_pptx(dd, text);
  fputs("</a:r>", pptx_obj->file );
  fputs("</a:p>", pptx_obj->file );
  fputs("</p:txBody>", pptx_obj->file );
}
// Callback functions for graphics device --------------------------------------

static void pptx_metric_info(int c, const pGEcontext gc, double* ascent,
                            double* descent, double* width, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  // Convert to string - negative implies unicode code point
  char str[16];
  if (c < 0) {
    Rf_ucstoutf8(str, (unsigned int) -c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  std::string file = fontfile(gc->fontfamily, gc->fontface, pptx_obj->user_aliases);
  std::string name = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases, pptx_obj->user_aliases);
  gdtools::context_set_font(pptx_obj->cc, name, gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(pptx_obj->cc, std::string(str));

  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

static void pptx_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  pptx_obj->clipleft = x0;
  pptx_obj->clipright = x1;
  pptx_obj->clipbottom = y0;
  pptx_obj->cliptop = y1;
  pptx_obj->clp->set_clipping_region(x0, y1, x1, y0);
}


static void pptx_close(pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  fprintf( pptx_obj->file, "%s", main_tree::a_closing_tag().c_str() );
  delete(pptx_obj);
}



static double pptx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  std::string file = fontfile(gc->fontfamily, gc->fontface, pptx_obj->user_aliases);
  std::string name = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases, pptx_obj->user_aliases);
  gdtools::context_set_font(pptx_obj->cc, name, gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(pptx_obj->cc, std::string(str));

  return fm.width;
}


static double pptx_strheight(const char *str, const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  std::string file = fontfile(gc->fontfamily, gc->fontface, pptx_obj->user_aliases);
  std::string name = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases, pptx_obj->user_aliases);
  gdtools::context_set_font(pptx_obj->cc, name, gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(pptx_obj->cc, std::string(str));
  return fm.height;
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
  fprintf(pptx_obj->file, "%s", a_path::a_tag(x, y, 0 ).c_str());
  fputs( "</a:pathLst>", pptx_obj->file );
  fputs( "</a:custGeom>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s",empty_body_text::p_tag().c_str());
  fputs("</p:sp>", pptx_obj->file);
}



static void pptx_line(double x1, double y1, double x2, double y2,
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


static void pptx_polyline(int n, double *x, double *y, const pGEcontext gc,
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

static void pptx_polygon(int n, double *x, double *y, const pGEcontext gc,
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
          fprintf(pptx_obj->file, "%s", a_path::a_tag(x__, y__, 1 ).c_str());
        fputs( "</a:pathLst>", pptx_obj->file );
      fputs("</a:custGeom>", pptx_obj->file );
      if( fill_.is_visible() > 0 )
        fprintf(pptx_obj->file, "%s", fill_.solid_fill().c_str());
      fprintf(pptx_obj->file, "%s", line_style_.a_tag().c_str());
    fputs("</p:spPr>", pptx_obj->file);
    fprintf(pptx_obj->file, "%s",empty_body_text::p_tag().c_str());
  fputs("</p:sp>", pptx_obj->file);
}

static void pptx_rect(double x0, double y0, double x1, double y1,
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
    fprintf(pptx_obj->file, "%s",empty_body_text::p_tag().c_str());
  fputs("</p:sp>", pptx_obj->file);
}

static void pptx_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
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
    fprintf(pptx_obj->file, "%s",empty_body_text::p_tag().c_str());

  fputs("</p:sp>", pptx_obj->file);
}


static void pptx_text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  double fs = gc->cex * gc->ps ;
  double w = pptx_strwidth(str, gc, dd);
  double h = pptx_strheight(str, gc, dd);
  if( fs*100 < 1.0 ) return;

  double corrected_offx = translate_rotate_x(x, y, rot, h, w, hadj) ;
  double corrected_offy = translate_rotate_y(x, y, rot, h, w, hadj) ;

  xfrm xfrm_(pptx_obj->offx + corrected_offx, pptx_obj->offy + corrected_offy, w, h, -rot );


  fputs("<p:sp>", pptx_obj->file);
    write_nv_pr_pptx(dd, "tx");
    fputs("<p:spPr>", pptx_obj->file);
      fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
      fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
      fputs("<a:noFill/>", pptx_obj->file);
    fputs("</p:spPr>", pptx_obj->file);

    write_text_body_pptx(dd, gc, str, hadj, fs, h);
  fputs("</p:sp>", pptx_obj->file);
}

static void pptx_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static void pptx_raster(unsigned int *raster, int w, int h,
                       double x, double y,
                       double width, double height,
                       double rot,
                       Rboolean interpolate,
                       const pGEcontext gc, pDevDesc dd)
{
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  std::stringstream os;
  int idx = pptx_obj->new_id();
  int id_img_rel = pptx_obj->nex_id_rel();

  os << pptx_obj->raster_prefix << "rId";
  os.fill('0');
  os.width(6);
  os << id_img_rel;
  os << ".png";


  if (height < 0)
    height = -height;
  xfrm xfrm_(pptx_obj->offx + x, pptx_obj->offy + y - height, width, height, -rot );

  std::vector<unsigned int> raster_(w*h);
  for (std::vector<unsigned int>::size_type i = 0 ; i < raster_.size(); ++i) {
    raster_[i] = raster[i] ;
  }
  gdtools::raster_to_file(raster_, w, h, width*(25/6), height*(25/6), interpolate, os.str());
  fputs("<p:pic>", pptx_obj->file);
    fputs("<p:nvPicPr>", pptx_obj->file);
      fprintf(pptx_obj->file,
        "<p:cNvPr id=\"%d\" name=\"pic%d\"/>",
        idx, idx );
      fputs("<p:cNvPicPr/>", pptx_obj->file);
      fputs("<p:nvPr/>", pptx_obj->file);
    fputs("</p:nvPicPr>", pptx_obj->file);
    fputs("<p:blipFill>", pptx_obj->file);
    fprintf(pptx_obj->file,
      "<a:blip r:embed=\"rId%d\" cstate=\"print\"/>",
      id_img_rel);
    fputs("<a:stretch><a:fillRect/></a:stretch>", pptx_obj->file);
    fputs("</p:blipFill>", pptx_obj->file);

    fputs("<p:spPr>", pptx_obj->file);
    fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
    fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
    fputs("</p:spPr>", pptx_obj->file);
  fputs("</p:pic>", pptx_obj->file);
}


static void pptx_new_page(const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  if (pptx_obj->pageno > 0) {
    Rf_error("pptx device only supports one page");
  }

  pptx_obj->clp->set_clipping_region(0.0, 0.0, dd->right, dd->bottom);
  main_tree mt(pptx_obj->new_id(), pptx_obj->new_id(),
               pptx_obj->offx, pptx_obj->offy,
               dd->right, dd->bottom, pptx_obj->standalone);

  fprintf(pptx_obj->file, "%s", mt.a_opening_tag().c_str() );

  int bg_fill, fill, col;
  a_color bg_temp(gc->fill);
  if (bg_temp.is_visible())
    bg_fill = gc->fill;
  else bg_fill = dd->startfill;

  a_color bg_color(bg_fill);
  if( bg_color.is_transparent() < 1 ){
    fill = gc->fill;
    col = gc->col;
    gc->fill = bg_fill;
    gc->col = bg_fill;
    pptx_rect(0, 0, dd->right, dd->bottom, gc, dd);
    gc->fill = fill;
    gc->col = col;
  }



  pptx_obj->pageno++;
}

pDevDesc pptx_driver_new(std::string filename, int bg, double width, double height,
                         double offx, double offy,
                        int pointsize,
                        Rcpp::List aliases,
                        bool editable, int id,
                        std::string raster_prefix,
                        int last_rel_id,
                        int standalone) {

  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;

  dd->startfill = bg;
  dd->startcol = R_RGB(0, 0, 0);
  dd->startps = pointsize;
  dd->startlty = 0;
  dd->startfont = 1;
  dd->startgamma = 1;

  // Callbacks
  dd->activate = NULL;
  dd->deactivate = NULL;
  dd->close = pptx_close;
  dd->clip = pptx_clip;
  dd->size = pptx_size;
  dd->newPage = pptx_new_page;
  dd->line = pptx_line;
  dd->text = pptx_text;
  dd->strWidth = pptx_strwidth;
  dd->rect = pptx_rect;
  dd->circle = pptx_circle;
  dd->polygon = pptx_polygon;
  dd->polyline = pptx_polyline;
  dd->path = NULL;
  dd->mode = NULL;
  dd->metricInfo = pptx_metric_info;
  dd->cap = NULL;
  dd->raster = pptx_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = pptx_text;
  dd->strWidthUTF8 = pptx_strwidth;

  // Screen Dimensions in pts
  dd->left = 0;
  dd->top = 0;
  dd->right = width * 72;
  dd->bottom = height * 72;

  // Magic constants copied from other graphics devices
  // nominal character sizes in pts
  dd->cra[0] = 0.9 * pointsize;
  dd->cra[1] = 1.2 * pointsize;
  // character alignment offsets
  dd->xCharOffset = 0.4900;
  dd->yCharOffset = 0.3333;
  dd->yLineBias = 0.2;
  // inches per pt
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 72.0;

  // Capabilities
  dd->canClip = TRUE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;
  dd->haveTransparency = 2;
  dd->haveTransparentBg = 2;

  dd->deviceSpecific = new PPTX_dev(filename,
                                    aliases,
    editable, offx*72, offy*72, id,
    raster_prefix,
    last_rel_id, standalone,
    width * 72,
    height * 72
    );
  return dd;
}

// [[Rcpp::export]]
bool PPTX_(std::string file, std::string bg_, double width, double height,
    double offx, double offy,
    int pointsize,
    Rcpp::List aliases,
    bool editable, int id,
    std::string raster_prefix,
    int last_rel_id, int standalone) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = pptx_driver_new(file, bg, width, height, offx, offy, pointsize,
                                   aliases,
      editable,
      id,
      raster_prefix,
      last_rel_id, standalone);
    if (dev == NULL)
      Rcpp::stop("Failed to start pptx device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "pptx_device");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}


