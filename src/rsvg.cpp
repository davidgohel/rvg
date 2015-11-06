/*
 * This file is part of rvg.
 * Copyright (c) 2014, David Gohel All rights reserved.
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
#include "R_ext/GraphicsEngine.h"

// SVG device metadata
class svgdev {
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
   /*   */

  XPtrCairoContext cc;

  svgdev(std::string filename_, bool standalone_, int canvas_id_ ):
      filename(filename_),
      pageno(0),
	    id(-1),
	    canvas_id(canvas_id_),
      standalone(standalone_),
      tracer_on(0),
      tracer_is_init(0),
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

  ~svgdev() {
    if (ok())
      fclose(file);
  }
};


inline bool is_bold(int face) {
  return face == 2 || face == 4;
}
inline bool is_italic(int face) {
  return face == 3 || face == 4;
}

inline std::string fontname(const char* family_, int face) {
  std::string family(family_);
  if( face == 5 ) return "symbol";

  if (family == "mono") {
    return "courier";
  } else if (family == "serif") {
    return "Times New Roman";
  } else if (family == "sans" || family == "") {
    return "Arial";
  } else {
    return family;
  }
}

void write_escaped(FILE* f, const char* text) {
  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", f); break;
    case '<': fputs("&lt;",  f); break;
    case '>': fputs("&gt;",  f); break;
    default:  fputc(*cur,    f);
    }
  }
}

void write_attr_col(FILE* f, const char* attr, unsigned int col) {
  int alpha = R_ALPHA(col);

  if (col == NA_INTEGER || alpha == 0) {
    fprintf(f, " %s='none'", attr);
    return;
  } else {
    fprintf(f, " %s='#%02X%02X%02X'", attr, R_RED(col), R_GREEN(col), R_BLUE(col));
    if (alpha != 255)
      fprintf(f, " %s-opacity='%0.2f'", attr, alpha / 255.0);
  }
}

void write_attr_dbl(FILE* f, const char* attr, double value) {
  fprintf(f, " %s='%.2f'", attr, value);
}

void write_attr_str(FILE* f, const char* attr, const char* value) {
  fprintf(f, " %s='%s'", attr, value);
}

static void write_attrs_linetype(FILE* f, int lty, double lwd, int col) {
  write_attr_col(f, "stroke", col);

  // 1 lwd = 1/96", but units in rest of document are 1/72"
  write_attr_dbl(f, "stroke-width", lwd / 96 * 72);

  // Set line pattern type
  switch (lty) {
  case LTY_BLANK: // never called: blank lines never get to this point
  case LTY_SOLID: // default svg setting, so don't need to write out
    break;
  default:
    // See comment in GraphicsEngine.h for how this works
    fputs(" stroke-dasharray='", f);
    for(int i = 0 ; i < 8 && lty & 15; i++) {
      fprintf(f, "%i ", (int) lwd * (lty & 15));
      lty = lty >> 4;
    }
    fputs("'", f);
    break;
  }
}

// Callback functions for graphics device --------------------------------------

static void svgd_metric_info(int c, const pGEcontext gc, double* ascent,
                            double* descent, double* width, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  // Convert to string - negative implies unicode code point
  char str[16];
  if (c < 0) {
    Rf_ucstoutf8(str, (unsigned int) -c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  gdtools::context_set_font(svgd->cc, fontname(gc->fontfamily, gc->fontface),
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

static void svgd_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  svgd->clipleft = x0;
  svgd->clipright = x1;
  svgd->clipbottom = y0;
  svgd->cliptop = y1;
}

static void svgd_new_page(const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  if (svgd->pageno > 0) {
    Rf_error("RVG only supports one page");
  }
  svgd->new_id();
  svgd->new_canvas_id();

  if (svgd->standalone)
    fputs("<?xml version='1.0' encoding='UTF-8' ?>\n", svgd->file);

  fputs("<svg ", svgd->file);
  if (svgd->standalone){
    fputs("xmlns='http://www.w3.org/2000/svg' ", svgd->file);
    //http://www.w3.org/wiki/SVG_Links
    fputs("xmlns:xlink='http://www.w3.org/1999/xlink' ", svgd->file);
  }

  fprintf(svgd->file, "id='svg_%d' ", svgd->canvas_id);
  fprintf(svgd->file, "viewBox='0 0 %.2f %.2f'>\n", dd->right, dd->bottom);

  fputs("<rect width='100%' height='100%'", svgd->file);
  write_attr_col(svgd->file, "fill", gc->fill);
  fputs("/>\n", svgd->file);

  svgd->pageno++;
}

static void svgd_close(pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  if (svgd->pageno > 0)
    fputs("</svg>\n", svgd->file);

  delete(svgd);
}

static void svgd_line(double x1, double y1, double x2, double y2,
                     const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();
  fprintf(svgd->file, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f' id='%d'",
    x1, y1, x2, y2, idx);

  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

void svgd_poly(int n, double *x, double *y, int filled, const pGEcontext gc,
              pDevDesc dd) {


  svgdev *svgd = (svgdev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();
  fputs("<polyline points='", svgd->file);

  for (int i = 0; i < n; i++) {
    fprintf(svgd->file, "%.2f,%.2f ", x[i], y[i]);
  }
  fputs("'", svgd->file);
  fprintf(svgd->file, " id='%d'", idx);
  write_attr_col(svgd->file, "fill", filled ? gc->fill : NA_INTEGER);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);

  fputs(" />\n", svgd->file);
}

static void svgd_polyline(int n, double *x, double *y, const pGEcontext gc,
                         pDevDesc dd) {
  svgd_poly(n, x, y, 0, gc, dd);
}
static void svgd_polygon(int n, double *x, double *y, const pGEcontext gc,
                        pDevDesc dd) {
  svgd_poly(n, x, y, 1, gc, dd);
}

void svgd_path(double *x, double *y,
              int npoly, int *nper,
              Rboolean winding,
              const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;
  // Create path data
  int idx = svgd->new_id();
  svgd->register_element();
  fputs("<path d='", svgd->file);

  int ind = 0;
  for (int i = 0; i < npoly; i++) {
    // Move to the first point of the sub-path
    fprintf(svgd->file, "M %.2f %.2f ", x[ind], y[ind]);
    ind++;
    // Draw the sub-path
    for (int j = 1; j < nper[i]; j++) {
      fprintf(svgd->file, "L %.2f %.2f ", x[ind], y[ind]);
      ind++;
    }
    // Close the sub-path
    fputs("Z ", svgd->file);
  }
  // Finish path data
  fputs("'", svgd->file);
  fprintf(svgd->file, " id='%d'", idx);
  write_attr_col(svgd->file, "fill", gc->fill);

  // Specify fill rule
  if (winding)
    fputs(" fill-rule='nonzero'", svgd->file);
  else
    fputs(" fill-rule='evenodd'", svgd->file);

  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);

  fputs(" />\n", svgd->file);
}

static double svgd_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  gdtools::context_set_font(svgd->cc, fontname(gc->fontfamily, gc->fontface),
    gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface));
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  return fm.width;
}

static void svgd_rect(double x0, double y0, double x1, double y1,
                     const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();

  // x and y give top-left position
  fprintf(svgd->file,
      "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f'",
      fmin(x0, x1), fmin(y0, y1), fabs(x1 - x0), fabs(y1 - y0));
  fprintf(svgd->file, " id='%d'", idx);
  write_attr_col(svgd->file, "fill", gc->fill);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

static void svgd_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;
  int idx = svgd->new_id();
  svgd->register_element();

  fprintf(svgd->file, "<circle cx='%.2f' cy='%.2f' r='%.2f'", x, y, r );
  fprintf(svgd->file, " id='%d'", idx);
  write_attr_col(svgd->file, "fill", gc->fill);
  write_attrs_linetype(svgd->file, gc->lty, gc->lwd, gc->col);
  fputs(" />\n", svgd->file);
}

static void svgd_text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {
  svgdev *svgd = (svgdev*) dd->deviceSpecific;

  int idx = svgd->new_id();
  svgd->register_element();

  fputs("<text", svgd->file);
  if (rot == 0) {
    write_attr_dbl(svgd->file, "x", x);
    write_attr_dbl(svgd->file, "y", y);
  } else {
    fprintf(svgd->file, " transform='translate(%.2f,%.2f) rotate(%0.0f)'", x, y,
      -1.0 * rot);
  }
  fprintf(svgd->file, " id='%d'", idx);

  write_attr_dbl(svgd->file, "font-size", gc->cex * gc->ps);
  if (is_bold(gc->fontface))
    write_attr_str(svgd->file, "font-weight", "bold");
  if (is_italic(gc->fontface))
    write_attr_str(svgd->file, "font-style", "italic");
  if (gc->col != -16777216) // black
    write_attr_col(svgd->file, "fill", gc->col);

  std::string font = fontname(gc->fontfamily, gc->fontface);
  write_attr_str(svgd->file, "font-family", font.c_str());

  fputs(">", svgd->file);

  write_escaped(svgd->file, str);

  fputs("</text>\n", svgd->file);
}

static void svgd_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static void svgd_raster(unsigned int *raster, int w, int h,
                       double x, double y,
                       double width, double height,
                       double rot,
                       Rboolean interpolate,
                       const pGEcontext gc, pDevDesc dd)
{
  svgdev *svgd = (svgdev*) dd->deviceSpecific;
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


pDevDesc svgd_driver_new(std::string filename, int bg, double width,
                        double height, int pointsize, bool standalone, int canvas_id) {

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
  dd->close = svgd_close;
  dd->clip = svgd_clip;
  dd->size = svgd_size;
  dd->newPage = svgd_new_page;
  dd->line = svgd_line;
  dd->text = svgd_text;
  dd->strWidth = svgd_strwidth;
  dd->rect = svgd_rect;
  dd->circle = svgd_circle;
  dd->polygon = svgd_polygon;
  dd->polyline = svgd_polyline;
  dd->path = svgd_path;
  dd->mode = NULL;
  dd->metricInfo = svgd_metric_info;
  dd->cap = NULL;
  dd->raster = svgd_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = svgd_text;
  dd->strWidthUTF8 = svgd_strwidth;

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

  dd->deviceSpecific = new svgdev(filename, standalone, canvas_id);
  return dd;
}

// [[Rcpp::export]]
bool devSVG_(std::string file, std::string bg_, int width, int height,
             int pointsize, bool standalone, int canvas_id) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = svgd_driver_new(file, bg, width, height, pointsize, standalone, canvas_id);
    if (dev == NULL)
      Rcpp::stop("Failed to start SVG2 device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "devRVG");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}


// [[Rcpp::export]]
bool set_tracer_on(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);
  if (dev) {
    svgdev *pd = (svgdev *) dev->dev->deviceSpecific;
    pd->set_tracer_on();
  }
  return true;
}

// [[Rcpp::export]]
bool set_tracer_off(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);
  if (dev) {
    svgdev *pd = (svgdev *) dev->dev->deviceSpecific;
    pd->set_tracer_off();
  }
  return true;
}

// [[Rcpp::export]]
Rcpp::IntegerVector collect_id(int dn) {
  pGEDevDesc dev= GEgetDevice(dn);

  if (dev) {
    svgdev *pd = (svgdev *) dev->dev->deviceSpecific;

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

  svgdev *svgd = (svgdev *) dev->dev->deviceSpecific;

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

  svgdev *svgd = (svgdev *) dev->dev->deviceSpecific;

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

  svgdev *svgd = (svgdev *) dev->dev->deviceSpecific;

  fputs("<script type='text/javascript'><![CDATA[", svgd->file);
  for( int i = 0 ; i < nb_elts ; i++ ){
    fprintf(svgd->file, "$('#svg_%d').find('#%d').attr('data-id','%s');",
      svgd->canvas_id, id[i], data_id[i].c_str() );
  }
  fputs("]]></script>", svgd->file);
  return true;
}
