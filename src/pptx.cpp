/*
 * This file is part of rvg.
 * Copyright (c) 2018, David Gohel All rights reserved.
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
#include <string.h>
#include <iostream>
#include "R_ext/GraphicsEngine.h"
#include "a_color.h"
#include "main_tree.h"
#include "shapes.h"
#include "raster.h"
#include "ooxml_dev.h"
#include "text.h"



// Callback functions for graphics device --------------------------------------

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
  fprintf( pptx_obj->file, "%s", main_tree::a_closing_tag(pptx_obj->standalone).c_str() );
  delete(pptx_obj);
}



static void pptx_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}


static SEXP pptx_setPattern(SEXP pattern, pDevDesc dd) {
    return R_NilValue;
}

static void pptx_releasePattern(SEXP ref, pDevDesc dd) {}

static SEXP pptx_setClipPath(SEXP path, SEXP ref, pDevDesc dd) {
    return R_NilValue;
}

static void pptx_releaseClipPath(SEXP ref, pDevDesc dd) {}

static SEXP pptx_setMask(SEXP path, SEXP ref, pDevDesc dd) {
    return R_NilValue;
}

static void pptx_releaseMask(SEXP ref, pDevDesc dd) {}

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
  dd->path = pptx_path;
  dd->mode = NULL;
  dd->metricInfo = pptx_metric_info;
  dd->cap = NULL;
  dd->raster = pptx_raster;
#if R_GE_version >= 13
  dd->setPattern      = pptx_setPattern;
  dd->releasePattern  = pptx_releasePattern;
  dd->setClipPath     = pptx_setClipPath;
  dd->releaseClipPath = pptx_releaseClipPath;
  dd->setMask         = pptx_setMask;
  dd->releaseMask     = pptx_releaseMask;
#endif

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = pptx_text_utf8;
  dd->strWidthUTF8 = pptx_strwidth_utf8;

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

#if R_GE_version >= 13
        dd->deviceVersion = R_GE_definitions;
#endif

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


