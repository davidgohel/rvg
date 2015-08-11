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
#include <cairo.h>

typedef struct {
	char *fontname;
	int ascent[4];
	int descent[4];
	int height[4];
	int isinit;
} FontInfo;



typedef struct {
	int first_elt;
	int last_elt;
	int on;
	int isinit;
} ElementTracer;

typedef struct {
	char *filename;
	char *fontname;
	int canvas_id;
	int canvas_start_id;
	FILE *dmlFilePointer;
	int fontface;
	int fontsize;
	int id;
    double clippedx0, clippedy0, clippedx1, clippedy1;
	double width;
	double height;
	FontInfo *fi;
	cairo_surface_t *surface;
	cairo_t *cr;
	ElementTracer *elt_tracer;

} DOCDesc;



