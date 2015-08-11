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

#include <Rinternals.h>
#include <R.h>

#include <R_ext/GraphicsEngine.h>
#include "R_ext/GraphicsDevice.h"
#include <R_ext/Boolean.h>

#include <cairo.h>

extern "C" {

static Rboolean rvgDeviceDriver(pDevDesc dev, const char* filename, double width,
		double height, double ps,
		const char* fontname, int canvas_id);

void GE_rvgDevice(const char* filename, double width, double height,
		double ps, const char* fontfamily, int canvas_id);

static void rvg_activate(pDevDesc dev);

static void rvg_Circle(double x, double y, double r, const pGEcontext gc,
		pDevDesc dev);

static void rvg_Line(double x1, double y1, double x2, double y2,
		const pGEcontext gc, pDevDesc dev) ;

static void rvg_Polyline(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) ;

static void rvg_Polygon(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) ;

static void rvg_Rect(double x0, double y0, double x1, double y1,
		const pGEcontext gc, pDevDesc dev) ;

static void rvg_Text(double x, double y, const char *str, double rot,
		double hadj, const pGEcontext gc, pDevDesc dev);

static void rvg_NewPage(const pGEcontext gc, pDevDesc dev) ;
static void rvg_Close(pDevDesc dev) ;
static void rvg_Clip(double x0, double x1, double y0, double y1, pDevDesc dev);

static void rvg_MetricInfo(int c, const pGEcontext gc, double* ascent,
		double* descent, double* width, pDevDesc dev);
static void rvg_Size(double *left, double *right, double *bottom, double *top,
		pDevDesc dev);

static double rvg_StrWidth(const char *str, const pGEcontext gc, pDevDesc dev);

//SEXP setDimensions(SEXP devNumber, SEXP ext, SEXP off);

SEXP R_rvg_Device(SEXP filename, SEXP width, SEXP height,
		SEXP pointsize, SEXP fontfamily, SEXP canvas_id);
};

