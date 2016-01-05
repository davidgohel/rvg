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
 * along with rvg.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "Rcpp.h"
#include <gdtools.h>
#include <string.h>
#include "fonts.h"
#include "R_ext/GraphicsEngine.h"
#include "a_color.h"
#include "line_style.h"

// SVG device metadata
class DSVG_dev {
public:
  FILE *file;
  std::string filename;
  int pageno;
  int id;
  int canvas_id;
  double clipleft, clipright, cliptop, clipbottom;
  bool standalone;
  /*   */
  int tracer_first_elt;
  int tracer_last_elt;
  int tracer_on;
  int tracer_is_init;

  std::string fontname_serif;
  std::string fontname_sans;
  std::string fontname_mono;
  std::string fontname_symbol;

  XPtrCairoContext cc;

  DSVG_dev(std::string filename_, bool standalone_, int canvas_id_,
           int bg_,
           std::string fontname_serif_,
           std::string fontname_sans_,
           std::string fontname_mono_,
           std::string fontname_symbol_ ):
      filename(filename_),
      pageno(0),
	    id(-1),
	    canvas_id(canvas_id_),
      standalone(standalone_),
      tracer_on(0),
      tracer_is_init(0),
      fontname_serif(fontname_serif_),
      fontname_sans(fontname_sans_),
      fontname_mono(fontname_mono_),
      fontname_symbol(fontname_symbol_),
      cc(gdtools::context_create()) {
    file = fopen(R_ExpandFileName(filename.c_str()), "w");
  }

  bool ok() const {
    return file != NULL;
  }

  int new_id() {
  	id++;
  	return id;
  }
  int new_canvas_id() {
    canvas_id++;
    return canvas_id;
  }

  void set_tracer_on(){
    tracer_on = 1;
    tracer_is_init = 0;
    tracer_first_elt = -1;
    tracer_last_elt = -1;
  }

  void set_tracer_off(){
    tracer_on = 0;
    tracer_is_init = 0;
    tracer_first_elt = -1;
    tracer_last_elt = -1;
  }

  void register_element() {
    if( tracer_on > 0 ){
      if( tracer_is_init < 1 ){
        tracer_first_elt = id;
        tracer_is_init = 1;
      }
      tracer_last_elt = id;
    }
  }

  ~DSVG_dev() {
    if (ok())
      fclose(file);
  }
};

static void dsvg_metric_info(int c, const pGEcontext gc, double* ascent,
                            double* descent, double* width, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  // Convert to string - negative implies unicode code point
  char str[16];
  if (c < 0) {
    Rf_ucstoutf8(str, (unsigned int) -c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  std::string fn = fontname(
    gc->fontfamily, gc->fontface,
    svgd->fontname_serif, svgd->fontname_sans,
    svgd->fontname_mono, svgd->fontname_symbol);

  gdtools::context_set_font(svgd->cc, fn,
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

static void dsvg_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  svgd->clipleft = x0;
  svgd->clipright = x1;
  svgd->clipbottom = y0;
  svgd->cliptop = y1;
}

static void dsvg_close(pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  if (svgd->pageno > 0)
    fputs("</svg>\n", svgd->file);

  delete(svgd);
}

static void dsvg_line(double x1, double y1, double x2, double y2,
                     const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();
  fprintf(svgd->file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' id='%d'",
    x1, y1, x2, y2, idx);
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fprintf(svgd->file, "%s", line_style_.svg_attr().c_str());
  a_color col_(gc->fill);
  fprintf(svgd->file, "%s", col_.svg_fill_attr().c_str());

  fputs("/>", svgd->file);
}

void dsvg_poly(int n, double *x, double *y, int filled, const pGEcontext gc,
              pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();
  fputs("<polyline points='", svgd->file);
  fprintf(svgd->file, "%.2f,%.2f", x[0], y[0]);
  for (int i = 1; i < n; i++) {
    fprintf(svgd->file, " %.2f,%.2f", x[i], y[i]);
  }
  fputs("'", svgd->file);
  fprintf(svgd->file, " id='%d'", idx);

  a_color col_(gc->fill);
  fprintf(svgd->file, "%s", col_.svg_fill_attr().c_str());


  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fprintf(svgd->file, "%s", line_style_.svg_attr().c_str());

  fputs("/>", svgd->file);
}

static void dsvg_polyline(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  dsvg_poly(n, x, y, 0, gc, dd);
}
static void dsvg_polygon(int n, double *x, double *y, const pGEcontext gc,
                        pDevDesc dd) {
  dsvg_poly(n, x, y, 1, gc, dd);
}

void dsvg_path(double *x, double *y,
              int npoly, int *nper,
              Rboolean winding,
              const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();
  fputs("<path d='", svgd->file);

  int ind = 0;
  for (int i = 0; i < npoly; i++) {
    fprintf(svgd->file, "M %.2f %.2f ", x[ind], y[ind]);
    ind++;
    for (int j = 1; j < nper[i]; j++) {
      fprintf(svgd->file, "L %.2f %.2f ", x[ind], y[ind]);
      ind++;
    }
    fputs("Z ", svgd->file);
  }
  fputs("'", svgd->file);
  fprintf(svgd->file, " id='%d'", idx);
  a_color fill_(gc->fill);
  fprintf(svgd->file, "%s", fill_.svg_fill_attr().c_str());

  if (winding)
    fputs(" fill-rule='nonzero'", svgd->file);
  else
    fputs(" fill-rule='evenodd'", svgd->file);

  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fprintf(svgd->file, "%s", line_style_.svg_attr().c_str());
  fputs("/>", svgd->file);
}

static double dsvg_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  std::string fn = fontname(
    gc->fontfamily, gc->fontface,
    svgd->fontname_serif, svgd->fontname_sans,
    svgd->fontname_mono, svgd->fontname_symbol);

  gdtools::context_set_font(svgd->cc, fn,
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  return fm.width;
}

static void dsvg_rect(double x0, double y0, double x1, double y1,
                     const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();

  fprintf(svgd->file,
      "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f'",
      fmin(x0, x1), fmin(y0, y1), fabs(x1 - x0), fabs(y1 - y0));
  fprintf(svgd->file, " id='%d'", idx);
  a_color fill_(gc->fill);
  fprintf(svgd->file, "%s", fill_.svg_fill_attr().c_str());
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fprintf(svgd->file, "%s", line_style_.svg_attr().c_str());

  fputs("/>", svgd->file);
}

static void dsvg_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();

  fprintf(svgd->file, "<circle cx='%.2f' cy='%.2f' r='%.2f'", x, y, r );
  fprintf(svgd->file, " id='%d'", idx);
  a_color fill_(gc->fill);
  fprintf(svgd->file, "%s", fill_.svg_fill_attr().c_str());
  line_style line_style_(gc->lwd, gc->col, gc->lty, gc->ljoin, gc->lend);
  fprintf(svgd->file, "%s", line_style_.svg_attr().c_str());
  fputs("/>", svgd->file);
}

static void dsvg_text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  int idx = svgd->new_id();
  svgd->register_element();

  fputs("<text", svgd->file);
  if (rot == 0) {
    fprintf(svgd->file, " x='%.2f' y='%.2f'", x, y);
  } else {
    fprintf(svgd->file, " transform='translate(%.2f,%.2f) rotate(%0.0f)'", x, y,
      -1.0 * rot);
  }
  fprintf(svgd->file, " id='%d'", idx);
  fprintf(svgd->file, " font-size='%.2fpt'", gc->cex * gc->ps);
  if (is_bold(gc->fontface))
    fputs(" font-weight='bold'", svgd->file);
  if (is_italic(gc->fontface))
    fputs(" font-style='italic'", svgd->file);
  if (gc->col != -16777216){
    a_color fill_(gc->col);
    fprintf(svgd->file, "%s", fill_.svg_fill_attr().c_str());
  } // black

  std::string font = fontname(
    gc->fontfamily, gc->fontface,
    svgd->fontname_serif, svgd->fontname_sans,
    svgd->fontname_mono, svgd->fontname_symbol);

  fprintf(svgd->file, " font-family='%s'", font.c_str());

  fputs(">", svgd->file);

  for(const char* cur = str; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", svgd->file); break;
    case '<': fputs("&lt;",  svgd->file); break;
    case '>': fputs("&gt;",  svgd->file); break;
    default:  fputc(*cur,    svgd->file);
    }
  }

  fputs("</text>", svgd->file);
}

static void dsvg_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static void dsvg_raster(unsigned int *raster, int w, int h,
                       double x, double y,
                       double width, double height,
                       double rot,
                       Rboolean interpolate,
                       const pGEcontext gc, pDevDesc dd)
{
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;
  int idx = svgd->new_id();

  if (height < 0)
    height = -height;

  std::vector<unsigned int> raster_(w*h);
  for ( int i = 0 ; i < raster_.size(); i++) {
    raster_[i] = raster[i] ;
  }

  std::string base64_str = gdtools::raster_to_str(raster_, w, h, width, height, (Rboolean) interpolate);

  fprintf(svgd->file, "<image x='%.2f' y='%.2f' ", x, y - height );
  fprintf(svgd->file, "width='%.2f' height='%.2f' ", width, height);
  fprintf(svgd->file, "id='%d' ", idx);

  if (fabs(rot)>0.001) {
    fprintf(svgd->file, "transform='rotate(%0.0f,%0.0f,%0.0f)' ", -1.0 * rot, x, y );
  }
  fprintf(svgd->file, "xlink:href='data:image/png;base64,%s'", base64_str.c_str());
  if (svgd->standalone) fputs( " xmlns:xlink='http://www.w3.org/1999/xlink'", svgd->file);
  fputs( "/>", svgd->file);


}

static void dsvg_new_page(const pGEcontext gc, pDevDesc dd) {
  DSVG_dev *svgd = (DSVG_dev*) dd->deviceSpecific;

  if (svgd->pageno > 0) {
    Rf_error("svgd only supports one page");
  }
  svgd->new_id();
  svgd->new_canvas_id();

  if (svgd->standalone)
    fputs("<?xml version='1.0' encoding='UTF-8'?>\n", svgd->file);

  fputs("<svg ", svgd->file);
  if (svgd->standalone){
    fputs("xmlns='http://www.w3.org/2000/svg' ", svgd->file);
    fputs("xmlns:xlink='http://www.w3.org/1999/xlink' ", svgd->file);
  }

  fprintf(svgd->file, "id='svg_%d' ", svgd->canvas_id);
  fprintf(svgd->file, "viewBox='0 0 %.2f %.2f'>", dd->right, dd->bottom);


  a_color bg_color(dd->startfill);
  if( bg_color.is_transparent() < 1 ){
    gc->fill = dd->startfill;
    gc->col = dd->startfill;
    int fill = gc->fill;
    int col = gc->col;
    dsvg_rect(0, 0, dd->right, dd->bottom, gc, dd);
    gc->fill = fill;
    gc->col = col;
  }

  svgd->pageno++;
}


pDevDesc dsvg_driver_new(std::string filename, int bg, double width,
                        double height, int pointsize, bool standalone, int canvas_id,
                        std::string fontname_serif,
                        std::string fontname_sans,
                        std::string fontname_mono,
                        std::string fontname_symbol) {

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
  dd->close = dsvg_close;
  dd->clip = dsvg_clip;
  dd->size = dsvg_size;
  dd->newPage = dsvg_new_page;
  dd->line = dsvg_line;
  dd->text = dsvg_text;
  dd->strWidth = dsvg_strwidth;
  dd->rect = dsvg_rect;
  dd->circle = dsvg_circle;
  dd->polygon = dsvg_polygon;
  dd->polyline = dsvg_polyline;
  dd->path = dsvg_path;
  dd->mode = NULL;
  dd->metricInfo = dsvg_metric_info;
  dd->cap = NULL;
  dd->raster = dsvg_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = dsvg_text;
  dd->strWidthUTF8 = dsvg_strwidth;

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
  dd->canClip = FALSE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;
  dd->haveTransparency = 2;
  dd->haveTransparentBg = 2;

  dd->deviceSpecific = new DSVG_dev(filename, standalone, canvas_id, bg,
                                  fontname_serif, fontname_sans, fontname_mono, fontname_symbol);
  return dd;
}

// [[Rcpp::export]]
bool DSVG_(std::string file, int width, int height, std::string bg,
             int pointsize, bool standalone, int canvas_id,
             std::string fontname_serif,
             std::string fontname_sans,
             std::string fontname_mono,
             std::string fontname_symbol) {

  int bg_ = R_GE_str2col(bg.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = dsvg_driver_new(file, bg_, width, height, pointsize, standalone, canvas_id,
                                   fontname_serif, fontname_sans, fontname_mono, fontname_symbol);
    if (dev == NULL)
      Rcpp::stop("Failed to start SVG2 device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "dsvg_device");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}


// [[Rcpp::export]]
bool set_tracer_on(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);
  if (dev) {
    DSVG_dev *pd = (DSVG_dev *) dev->dev->deviceSpecific;
    pd->set_tracer_on();
  }
  return true;
}

// [[Rcpp::export]]
bool set_tracer_off(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);
  if (dev) {
    DSVG_dev *pd = (DSVG_dev *) dev->dev->deviceSpecific;
    pd->set_tracer_off();
  }
  return true;
}

// [[Rcpp::export]]
Rcpp::IntegerVector collect_id(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);

  if (dev) {
    DSVG_dev *pd = (DSVG_dev *) dev->dev->deviceSpecific;

    int first = pd->tracer_first_elt;
    int last = pd->tracer_last_elt;

    if( first < 0 || last < 0 ){
      Rcpp::IntegerVector res(0);
      return res;
    } else if( first > last ){
      Rcpp::IntegerVector res(0);
      return res;
    }

    int l_ = 1 + last - first;

    Rcpp::IntegerVector res(l_);

    for( int i = first ; i <= last ; i++ ){
      res[i-first] = i;
    }
    return res;
  }
  return R_NilValue;
}

// [[Rcpp::export]]
bool add_tooltip(int dn, Rcpp::IntegerVector id, std::vector< std::string > str){
  int nb_elts = id.size();
  pGEDevDesc dev= GEgetDevice(dn);

  if (!dev) return false;

  DSVG_dev *svgd = (DSVG_dev *) dev->dev->deviceSpecific;

  fputs("<script type='text/javascript'><![CDATA[", svgd->file);

  for( int i = 0 ; i < nb_elts ; i++ ){

    fprintf(svgd->file, "$('#svg_%d').find('#%d').attr('data-toggle','tooltip').attr('title','%s')",
    svgd->canvas_id, id[i], str[i].c_str() );
    fputs(".attr('data-html','true').tooltip({'container':'body','placement':'bottom'});", svgd->file);
  }
  fputs("]]></script>", svgd->file);
  return true;
}


// [[Rcpp::export]]
bool add_click_event(int dn, Rcpp::IntegerVector id, std::vector< std::string > str){
  int nb_elts = id.size();
  pGEDevDesc dev= GEgetDevice(dn);

  if (!dev) return false;

  DSVG_dev *svgd = (DSVG_dev *) dev->dev->deviceSpecific;

  fputs("<script type='text/javascript'><![CDATA[", svgd->file);
  for( int i = 0 ; i < nb_elts ; i++ ){
    fprintf(svgd->file, "$('#svg_%d').find('#%d').click(%s);",
      svgd->canvas_id, id[i], str[i].c_str() );
  }
  fputs("]]></script>", svgd->file);
  return true;
}

// [[Rcpp::export]]
bool add_data_id(int dn, Rcpp::IntegerVector id, std::vector< std::string > data_id){
  int nb_elts = id.size();
  pGEDevDesc dev= GEgetDevice(dn);

  if (!dev) return false;

  DSVG_dev *svgd = (DSVG_dev *) dev->dev->deviceSpecific;

  fputs("<script type='text/javascript'><![CDATA[", svgd->file);
  for( int i = 0 ; i < nb_elts ; i++ ){
    fprintf(svgd->file, "$('#svg_%d').find('#%d').attr('data-id','%s');",
      svgd->canvas_id, id[i], data_id[i].c_str() );
  }
  fputs("]]></script>", svgd->file);
  return true;
}
