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

static void xlsx_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  xlsx_obj->clipleft = x0;
  xlsx_obj->clipright = x1;
  xlsx_obj->clipbottom = y0;
  xlsx_obj->cliptop = y1;
  xlsx_obj->clp->set_clipping_region(x0, y1, x1, y0);
}


static void xlsx_close(pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  fprintf( xlsx_obj->file, "%s", main_tree::x_closing_tag().c_str() );
  delete(xlsx_obj);
}





static void xlsx_size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

static SEXP xlsx_setPattern(SEXP pattern, pDevDesc dd) {
    return R_NilValue;
}

static void xlsx_releasePattern(SEXP ref, pDevDesc dd) {}

static SEXP xlsx_setClipPath(SEXP path, SEXP ref, pDevDesc dd) {
    return R_NilValue;
}

static void xlsx_releaseClipPath(SEXP ref, pDevDesc dd) {}

static SEXP xlsx_setMask(SEXP path, SEXP ref, pDevDesc dd) {
    return R_NilValue;
}

static void xlsx_releaseMask(SEXP ref, pDevDesc dd) {}

#if R_GE_version >= 15
static SEXP xlsx_defineGroup(SEXP source, int op, SEXP destination, pDevDesc dd) {
    return R_NilValue;
}
static void xlsx_useGroup(SEXP ref, SEXP trans, pDevDesc dd) {}
static void xlsx_releaseGroup(SEXP ref, pDevDesc dd) {}
static void xlsx_stroke(SEXP path, const pGEcontext gc, pDevDesc dd) {}
static void xlsx_fill(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}
static void xlsx_fillStroke(SEXP path, int rule, const pGEcontext gc, pDevDesc dd) {}
static SEXP xlsx_capabilities(SEXP capabilities) {
    SET_VECTOR_ELT(capabilities, R_GE_capability_patterns,
                   Rf_ScalarInteger(0));
    SET_VECTOR_ELT(capabilities, R_GE_capability_clippingPaths,
                   Rf_ScalarInteger(0));
    SET_VECTOR_ELT(capabilities, R_GE_capability_masks,
                   Rf_ScalarInteger(0));
    SET_VECTOR_ELT(capabilities, R_GE_capability_compositing,
                   Rf_ScalarInteger(0));
    SET_VECTOR_ELT(capabilities, R_GE_capability_transformations,
                   Rf_ScalarInteger(0));
    SET_VECTOR_ELT(capabilities, R_GE_capability_paths,
                   Rf_ScalarInteger(0));
    return capabilities;
}
#endif

static void xlsx_new_page(const pGEcontext gc, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;

  if (xlsx_obj->pageno > 0) {
    Rf_error("xlsx device only supports one page");
  }

  xlsx_obj->clp->set_clipping_region(0.0, 0.0, dd->right, dd->bottom);
  main_tree mt(xlsx_obj->new_id(), xlsx_obj->new_id(),
               xlsx_obj->offx, xlsx_obj->offy,
               dd->right, dd->bottom, xlsx_obj->standalone);

  fprintf(xlsx_obj->file, "%s", mt.x_opening_tag().c_str() );

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
    xlsx_rect(0, 0, dd->right, dd->bottom, gc, dd);
    gc->fill = fill;
    gc->col = col;
  }



  xlsx_obj->pageno++;
}

pDevDesc xlsx_driver_new(std::string filename, int bg, double width, double height,
                         double offx, double offy,
                        int pointsize,
                        Rcpp::List aliases,
                        bool editable, int id,
                        std::string raster_prefix,
                        int next_rels_id,
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
  dd->close = xlsx_close;
  dd->clip = xlsx_clip;
  dd->size = xlsx_size;
  dd->newPage = xlsx_new_page;
  dd->line = xlsx_line;
  dd->text = xlsx_text;
  dd->strWidth = xlsx_strwidth;
  dd->rect = xlsx_rect;
  dd->circle = xlsx_circle;
  dd->polygon = xlsx_polygon;
  dd->polyline = xlsx_polyline;
  dd->path = xlsx_path;
  dd->mode = NULL;
  dd->metricInfo = xlsx_metric_info;
  dd->cap = NULL;
  dd->raster = xlsx_raster;
#if R_GE_version >= 13
  dd->setPattern      = xlsx_setPattern;
  dd->releasePattern  = xlsx_releasePattern;
  dd->setClipPath     = xlsx_setClipPath;
  dd->releaseClipPath = xlsx_releaseClipPath;
  dd->setMask         = xlsx_setMask;
  dd->releaseMask     = xlsx_releaseMask;
#endif
#if R_GE_version >= 15
  dd->defineGroup     = xlsx_defineGroup;
  dd->useGroup        = xlsx_useGroup;
  dd->releaseGroup    = xlsx_releaseGroup;
  dd->stroke          = xlsx_stroke;
  dd->fill            = xlsx_fill;
  dd->fillStroke      = xlsx_fillStroke;
  dd->capabilities    = xlsx_capabilities;
#endif

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = xlsx_text_utf8;
  dd->strWidthUTF8 = xlsx_strwidth_utf8;

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

#if R_GE_version >= 15
        dd->deviceVersion = R_GE_group;
        dd->deviceClip = TRUE;
#elif R_GE_version >= 14
        dd->deviceVersion = R_GE_deviceClip;
        dd->deviceClip = TRUE;
#elif R_GE_version >= 13
        dd->deviceVersion = R_GE_definitions;
#endif

  dd->deviceSpecific = new XLSX_dev(filename,
                                    aliases,
                                    editable, offx*72, offy*72, id,
    raster_prefix,
    next_rels_id, standalone,
    width * 72,
    height * 72
    );
  return dd;
}

// [[Rcpp::export]]
bool XLSX_(std::string file, std::string bg_, double width, double height,
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
    pDevDesc dev = xlsx_driver_new(file, bg, width, height, offx, offy, pointsize,
                                   aliases,
                                   editable,
      id,
      raster_prefix,
      last_rel_id, standalone);
    if (dev == NULL)
      Rcpp::stop("Failed to start xlsx device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "xlsx_device");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;

  return true;
}


