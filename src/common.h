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


double getFontSize(double cex, double fontsize);

double getFontHeight(const char *str, const pGEcontext gc, pDevDesc dev);
int get_and_increment_idx(pDevDesc dev);
void register_element(pDevDesc dev);
void updateFontInfo(pDevDesc dev, R_GE_gcontext *gc);
void closeFile( FILE *file);

void DOC_ClipLine(double x0, double y0, double x1, double y1, pDevDesc dev);
void DOC_ClipRect(double x0, double y0, double x1, double y1,
		pDevDesc dev);
