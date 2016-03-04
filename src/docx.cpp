/*
 * This file is part of rvg.
 * Copyright (c) 2016, David Gohel All rights reserved.
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
class DOCX_dev {
public:
  FILE *file;
  std::string filename;
  int pageno;
  int id;
  double clipleft, clipright, cliptop, clipbottom;

  std::string raster_prefix;
  int img_id;

  std::string fontname_serif;
  std::string fontname_sans;
  std::string fontname_mono;
  std::string fontname_symbol;
  bool editable;
  int standalone;
  XPtrCairoContext cc;
  clipper *clp;

  DOCX_dev(std::string filename_,
           std::string fontname_serif_,
           std::string fontname_sans_,
           std::string fontname_mono_,
           std::string fontname_symbol_,
           bool editable_, int id_,
           std::string raster_prefix_,
           int next_rels_id_, int standalone_,
           double width_, double height_ ):
      filename(filename_),
      pageno(0),
	    id(id_),
	    raster_prefix(raster_prefix_), img_id(next_rels_id_),
	    fontname_serif(fontname_serif_),
	    fontname_sans(fontname_sans_),
	    fontname_mono(fontname_mono_),
	    fontname_symbol(fontname_symbol_),
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
    img_id++;
    return img_id;
  }
  ~DOCX_dev() {
    if (ok())
      fclose(file);
  }
};

void write_nv_pr_docx(pDevDesc dev, const char *label) {
  DOCX_dev *docx_obj = (DOCX_dev *) dev->deviceSpecific;
  int idx = docx_obj->new_id();
  nv_pr obj(idx, (int)(docx_obj->editable), label);
  fprintf(docx_obj->file, "%s", obj.wps_tag().c_str());
}


void write_t_docx(pDevDesc dev, const char* text) {
  DOCX_dev *docx_obj = (DOCX_dev *) dev->deviceSpecific;
  fputs("<w:t>", docx_obj->file );

  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", docx_obj->file); break;
    case '<': fputs("&lt;",  docx_obj->file); break;
    case '>': fputs("&gt;",  docx_obj->file); break;
    default:  fputc(*cur,    docx_obj->file);
    }
  }
  fputs("</w:t>", docx_obj->file );
}


void write_text_body_docx(pDevDesc dd, R_GE_gcontext *gc, const char* text, double hadj, double fontsize, double fontheight) {
  DOCX_dev *docx_obj = (DOCX_dev *) dd->deviceSpecific;
  ppr a_ppr_(hadj, fontsize);
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface,
    docx_obj->fontname_serif, docx_obj->fontname_sans,
    docx_obj->fontname_mono, docx_obj->fontname_symbol);

  rpr rpr_(fontsize, is_italic(gc->fontface), is_bold(gc->fontface), gc->col, fontname_);

  fputs("<wps:txbx>", docx_obj->file );
  fputs("<w:txbxContent>", docx_obj->file );

  fputs("<w:p>", docx_obj->file );
  fprintf(docx_obj->file, "%s", a_ppr_.w_tag().c_str() );
  fputs("<w:r>", docx_obj->file );
  fprintf(docx_obj->file, "%s", rpr_.w_tag().c_str() );
  write_t_docx(dd, text);
  fputs("</w:r>", docx_obj->file );
  fputs("</w:p>", docx_obj->file );
  fputs("</w:txbxContent>", docx_obj->file );
  fputs("</wps:txbx>", docx_obj->file );

  fprintf(docx_obj->file, "%s", body_pr::wps_tag().c_str() );

}
// Callback functions for graphics device --------------------------------------

static void docx_metric_info(int c, const pGEcontext gc, double* ascent,
                            double* descent, double* width, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  // Convert to string - negative implies unicode code point
  char str[16];
  if (c < 0) {
    Rf_ucstoutf8(str, (unsigned int) -c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  std::string fn = fontname(gc->fontfamily, gc->fontface,
           docx_obj->fontname_serif, docx_obj->fontname_sans,
           docx_obj->fontname_mono, docx_obj->fontname_symbol);

  gdtools::context_set_font(docx_obj->cc, fn,
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(docx_obj->cc, std::string(str));
  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

static void docx_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  docx_obj->clipleft = x0;
  docx_obj->clipright = x1;
  docx_obj->clipbottom = y0;
  docx_obj->cliptop = y1;
  docx_obj->clp->set_clipping_region(x0, y1, x1, y0);
}

static void docx_close(pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  fprintf( docx_obj->file, "%s", main_tree::w_closing_tag().c_str() );
  delete(docx_obj);
}



static double docx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  std::string fn = fontname(
    gc->fontfamily, gc->fontface,
    docx_obj->fontname_serif, docx_obj->fontname_sans,
    docx_obj->fontname_mono, docx_obj->fontname_symbol);

  gdtools::context_set_font(docx_obj->cc, fn,
                            gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(docx_obj->cc, std::string(str));

  return fm.width;
}


static double docx_strheight(const char *str, const pGEcontext gc, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  std::string fn = fontname(gc->fontfamily, gc->fontface,
                            docx_obj->fontname_serif, docx_obj->fontname_sans,
                            docx_obj->fontname_mono, docx_obj->fontname_symbol);

  gdtools::context_set_font(docx_obj->cc, fn,
                            gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(docx_obj->cc, std::string(str));
  return fm.height;
}



void docx_do_polyline(NumericVector x, NumericVector y, const pGEcontext gc,
                   pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  xfrm xfrm_(x, y );
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fputs("<wps:wsp>", docx_obj->file);
  write_nv_pr_docx(dd, "pl");
  fputs("<wps:spPr>", docx_obj->file);
  fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
  fputs( "<a:custGeom><a:avLst/>", docx_obj->file );
  fputs( "<a:pathLst>", docx_obj->file );
  fprintf(docx_obj->file, "%s", a_path::a_tag(x, y, 0 ).c_str());
  fputs( "</a:pathLst>", docx_obj->file );
  fputs( "</a:custGeom>", docx_obj->file );
  fprintf(docx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</wps:spPr>", docx_obj->file);
  fprintf(docx_obj->file, "%s",empty_body_text::wps_tag().c_str());
  fputs("</wps:wsp>", docx_obj->file);
}

static void docx_line(double x1, double y1, double x2, double y2,
                      const pGEcontext gc, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  Rcpp::NumericVector x_(2);
  Rcpp::NumericVector y_(2);
  x_[0] = x1 ;
  y_[0] = y1 ;
  x_[1] = x2 ;
  y_[1] = y2 ;

  docx_obj->clp->set_data(x_, y_);
  docx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = docx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = docx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    docx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }

}

static void docx_polyline(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  int i;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }
  docx_obj->clp->set_data(x_, y_);
  docx_obj->clp->clip_polyline();

  std::vector<NumericVector> x_array = docx_obj->clp->get_x_lines();
  std::vector<NumericVector> y_array = docx_obj->clp->get_y_lines();

  for( size_t l = 0 ; l < x_array.size() ; l++ ){
    docx_do_polyline(x_array.at(l), y_array.at(l), gc, dd);
  }

}

static void docx_polygon(int n, double *x, double *y, const pGEcontext gc,
                        pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  int i;
  Rcpp::NumericVector x_(n);
  Rcpp::NumericVector y_(n);

  for(i = 0 ; i < n ; i++ ){
    x_[i] = x[i];
    y_[i] = y[i];
  }
  docx_obj->clp->set_data(x_, y_);
  docx_obj->clp->clip_polygon();

  Rcpp::NumericVector x__ = docx_obj->clp->get_x();
  Rcpp::NumericVector y__ = docx_obj->clp->get_y();

  xfrm xfrm_(x__, y__);
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<wps:wsp>", docx_obj->file);
    write_nv_pr_docx(dd, "pg");
    fputs("<wps:spPr>", docx_obj->file);
      fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
      fputs("<a:custGeom><a:avLst/>", docx_obj->file );
        fputs( "<a:pathLst>", docx_obj->file );
          fprintf(docx_obj->file, "%s", a_path::a_tag(x__, y__, 1 ).c_str());
        fputs( "</a:pathLst>", docx_obj->file );
      fputs("</a:custGeom>", docx_obj->file );
      if( fill_.is_visible() > 0 )
        fprintf(docx_obj->file, "%s", fill_.solid_fill().c_str());
      fprintf(docx_obj->file, "%s", line_style_.a_tag().c_str());
    fputs("</wps:spPr>", docx_obj->file);
    fprintf(docx_obj->file, "%s",empty_body_text::wps_tag().c_str());
  fputs("</wps:wsp>", docx_obj->file);
}

static void docx_rect(double x0, double y0, double x1, double y1,
                     const pGEcontext gc, pDevDesc dd) {

  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  Rcpp::NumericVector x_(2);
  Rcpp::NumericVector y_(2);
  x_[0] = x0;
  y_[0] = y0;
  x_[1] = x1;
  y_[1] = y1;
  docx_obj->clp->set_data(x_, y_);
  docx_obj->clp->clip_polygon();
  Rcpp::NumericVector x__ = docx_obj->clp->get_x();
  Rcpp::NumericVector y__ = docx_obj->clp->get_y();

  xfrm xfrm_(x__, y__);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );

  fputs("<wps:wsp>", docx_obj->file);
    write_nv_pr_docx(dd, "rc");
    fputs("<wps:spPr>", docx_obj->file);
      fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
      fprintf(docx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
      if( fill_.is_visible() > 0 )
        fprintf(docx_obj->file, "%s", fill_.solid_fill().c_str());
      fprintf(docx_obj->file, "%s", line_style_.a_tag().c_str());
    fputs("</wps:spPr>", docx_obj->file);
    fprintf(docx_obj->file, "%s",empty_body_text::wps_tag().c_str());
  fputs("</wps:wsp>", docx_obj->file);
}

static void docx_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  a_color fill_( gc->fill );
  xfrm xfrm_(x - r, y - r, r * 2, r * 2 , 0.0 );


  fputs("<wps:wsp>", docx_obj->file);
    write_nv_pr_docx(dd, "pt");
    fputs("<wps:spPr>", docx_obj->file);
      fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
      fprintf(docx_obj->file,"%s", a_prstgeom::a_tag("ellipse").c_str());
      if( fill_.is_visible() > 0 )
        fprintf(docx_obj->file, "%s", fill_.solid_fill().c_str());
      fprintf(docx_obj->file, "%s", line_style_.a_tag().c_str());
    fputs("</wps:spPr>", docx_obj->file);
    fprintf(docx_obj->file, "%s",empty_body_text::wps_tag().c_str());

  fputs("</wps:wsp>", docx_obj->file);
}


static void docx_text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  double fs = gc->cex * gc->ps;
  double w = docx_strwidth(str, gc, dd)*1.02;
  double h = docx_strheight(str, gc, dd)*1.45;
  if( fs*2 < 1.0 ) return;

  double corrected_offx = translate_rotate_x(x, y, rot, h, w, hadj) ;
  double corrected_offy = translate_rotate_y(x, y, rot, h, w, hadj) ;

  xfrm xfrm_(corrected_offx, corrected_offy, w, h, -rot );


  fputs("<wps:wsp>", docx_obj->file);
    write_nv_pr_docx(dd, "tx");
    fputs("<wps:spPr>", docx_obj->file);
      fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
      fprintf(docx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
      fputs("<a:noFill/>", docx_obj->file);
    fputs("</wps:spPr>", docx_obj->file);

    write_text_body_docx(dd, gc, str, hadj, fs, h);
  fputs("</wps:wsp>", docx_obj->file);
}

static void docx_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static void docx_raster(unsigned int *raster, int w, int h,
                       double x, double y,
                       double width, double height,
                       double rot,
                       Rboolean interpolate,
                       const pGEcontext gc, pDevDesc dd)
{
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;
  docx_obj->img_id++;
  std::stringstream os;
  int id_img_rel = docx_obj->nex_id_rel();

  os << docx_obj->raster_prefix << "rId";
  os.fill('0');
  os.width(6);
  os << id_img_rel;
  os << ".png";


  if (height < 0)
    height = -height;
  xfrm xfrm_(x, y - height, width, height, -rot );
  line_style line_style_(0, 0, gc->lty, gc->ljoin, gc->lend);

  std::vector<unsigned int> raster_(w*h);
  for (std::vector<unsigned int>::size_type i = 0 ; i < raster_.size(); ++i) {
    raster_[i] = raster[i] ;
  }
  gdtools::raster_to_file(raster_, w, h, width, height, (Rboolean) interpolate, os.str());

  fputs("<wps:wsp>", docx_obj->file);
  write_nv_pr_docx(dd, "rs");
  fputs("<wps:spPr>", docx_obj->file);
  fprintf(docx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(docx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  fputs("<a:blipFill rotWithShape=\"1\">", docx_obj->file);
  fprintf(docx_obj->file,
          "<a:blip r:embed=\"rId%d\"/>",
          id_img_rel);
  fputs("<a:stretch><a:fillRect/></a:stretch>", docx_obj->file);
  fputs("</a:blipFill>", docx_obj->file);
  fprintf(docx_obj->file, "%s", line_style_.a_tag().c_str());
  fputs("</wps:spPr>", docx_obj->file);
  fprintf(docx_obj->file, "%s",empty_body_text::wps_tag().c_str());
  fputs("</wps:wsp>", docx_obj->file);
}

static void docx_new_page(const pGEcontext gc, pDevDesc dd) {
  DOCX_dev *docx_obj = (DOCX_dev*) dd->deviceSpecific;

  if (docx_obj->pageno > 0) {
    Rf_error("docx device only supports one page");
  }
  docx_obj->clp->set_clipping_region(0.0, 0.0, dd->right, dd->bottom);
  main_tree mt(docx_obj->new_id(),
               docx_obj->new_id(),
               0.0, 0.0,
               dd->right,
               dd->bottom, docx_obj->standalone);

  fprintf(docx_obj->file, "%s", mt.w_opening_tag().c_str() );

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
    docx_rect(0, 0, dd->right, dd->bottom, gc, dd);
    gc->fill = fill;
    gc->col = col;
  }

  docx_obj->pageno++;
}



pDevDesc docx_driver_new(std::string filename, int bg, double width, double height,
                        int pointsize,
                        std::string fontname_serif,
                        std::string fontname_sans,
                        std::string fontname_mono,
                        std::string fontname_symbol,
                        bool editable, int id,
                        std::string raster_prefix,
                        int next_rels_id, int standalone) {

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
  dd->close = docx_close;
  dd->clip = docx_clip;
  dd->size = docx_size;
  dd->newPage = docx_new_page;
  dd->line = docx_line;
  dd->text = docx_text;
  dd->strWidth = docx_strwidth;
  dd->rect = docx_rect;
  dd->circle = docx_circle;
  dd->polygon = docx_polygon;
  dd->polyline = docx_polyline;
  dd->path = NULL;
  dd->mode = NULL;
  dd->metricInfo = docx_metric_info;
  dd->cap = NULL;
  dd->raster = docx_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = docx_text;
  dd->strWidthUTF8 = docx_strwidth;

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

  dd->deviceSpecific = new DOCX_dev(filename,
    fontname_serif, fontname_sans, fontname_mono, fontname_symbol,
    editable, id,
    raster_prefix,
    next_rels_id, standalone,
    width * 72, height * 72);
  return dd;
}

// [[Rcpp::export]]
bool DOCX_(std::string file, std::string bg_, double width, double height,
    int pointsize,
    std::string fontname_serif,
    std::string fontname_sans,
    std::string fontname_mono,
    std::string fontname_symbol,
    bool editable, int id,
    std::string raster_prefix,
    int next_rels_id, int standalone) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = docx_driver_new(file, bg, width, height, pointsize,
      fontname_serif, fontname_sans, fontname_mono, fontname_symbol,
      editable,
      id,
      raster_prefix,
      next_rels_id,
      standalone);
    if (dev == NULL)
      Rcpp::stop("Failed to start docx device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "docx_device");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}


