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

#include <string.h>
#include <stdio.h>
#include <math.h>

#include <locale.h>
#include <R_ext/Riconv.h>
#include <errno.h>
#include "rvg.h"

#define R_USE_PROTOTYPES 1

#include "datastruct.h"
#include "common.h"
#include "utils.h"

#define CREDC(C) (((unsigned int)(C))&0xff)
#define CGREENC(C) ((((unsigned int)(C))&0xff00)>>8)
#define CBLUEC(C) ((((unsigned int)(C))&0xff0000)>>16)
#define CALPHA(C) ((((unsigned int)(C))&0xff000000)>>24)



static Rboolean rvgDeviceDriver(pDevDesc dev, const char* filename,
		double width, double height, double ps,
		const char* fontname, int canvas_id) {

	DOCDesc *rd;
	rd = (DOCDesc *) malloc(sizeof(DOCDesc));

	FontInfo *fi = (FontInfo *) malloc(sizeof(FontInfo));
	fi->isinit=0;
	fi->fontname=strdup(fontname);
	rd->fi = fi;

	ElementTracer *elt_tracer = (ElementTracer *) malloc(sizeof(ElementTracer));
	elt_tracer->on = 0;
	elt_tracer->isinit = 0;
	rd->elt_tracer = elt_tracer;
	rd->canvas_id = canvas_id;
	rd->canvas_start_id = canvas_id + 1;
	rd->filename = strdup(filename);
	rd->fontname = strdup(fontname);
	rd->id = 0;
	rd->width = width;
	rd->height = height;
	rd->fontface = 1;
	rd->fontsize = (int) ps;
	//
	//  Device functions
	//
	dev->deviceSpecific = rd;
	dev->activate = rvg_activate;
	dev->close = rvg_Close;
	dev->size = rvg_Size;
	dev->newPage = rvg_NewPage;
	dev->clip = rvg_Clip;
	dev->strWidth = rvg_StrWidth;
	dev->strWidthUTF8 = rvg_StrWidth;
	dev->text = rvg_Text;
	dev->textUTF8 = rvg_Text;
	dev->rect = rvg_Rect;
	dev->circle = rvg_Circle;
	dev->line = rvg_Line;
	dev->polyline = rvg_Polyline;
	dev->polygon = rvg_Polygon;
	dev->metricInfo = rvg_MetricInfo;
	dev->raster = rvg_Raster;
	dev->hasTextUTF8 = (Rboolean) TRUE;
	dev->wantSymbolUTF8 = (Rboolean) TRUE;
	dev->useRotatedTextInContour = (Rboolean) FALSE;
	/*
	 * Initial graphical settings
	 */
	dev->startfont = 1;
	dev->startps = ps;
	dev->startcol = R_RGB(0, 0, 0);
	dev->startfill = R_TRANWHITE;
	dev->startlty = LTY_SOLID;
	dev->startgamma = 1;


	/*
	 * Device physical characteristics
	 */

	dev->left = 0;
	dev->right = width;
	dev->bottom = height;
	dev->top = 0;

	dev->clipLeft = 0;
	dev->clipRight = width;
	dev->clipBottom = height;
	dev->clipTop = 0;

	rd->clippedx0 = dev->clipLeft;
	rd->clippedy0 = dev->clipTop;
	rd->clippedx1 = dev->clipRight;
	rd->clippedy1 = dev->clipBottom;

	dev->cra[0] = 0.9 * ps;
	dev->cra[1] = 1.2 * ps;
	dev->xCharOffset = 0.4900;
	dev->yCharOffset = 0.3333;
	//dev->yLineBias = 0.2;
	dev->ipr[0] = 1.0 / 72.2;
	dev->ipr[1] = 1.0 / 72.2;
	/*
	 * Device capabilities
	 */
	dev->canClip = (Rboolean) TRUE;
	dev->canHAdj = 2;
	dev->canChangeGamma = (Rboolean) FALSE;
	dev->displayListOn = (Rboolean) FALSE;

	dev->haveTransparency = 2;
	dev->haveTransparentBg = 3;

	return (Rboolean) TRUE;
}


void GE_rvgDevice(const char* filename, double width, double height,
		double ps, const char* fontfamily, int canvas_id) {
	pDevDesc dev = NULL;
	pGEDevDesc dd;
	R_GE_checkVersionOrDie (R_GE_version);
	R_CheckDeviceAvailable();

	if (!(dev = (pDevDesc) calloc(1, sizeof(DevDesc))))
		Rf_error("unable to start rvg device");
	if (!rvgDeviceDriver(dev, filename, width, height, ps,
			fontfamily, canvas_id)) {
		free(dev);
		Rf_error("unable to start rvg device");
	}

	dd = GEcreateDevDesc(dev);
	GEaddDevice2(dd, "rvg");

}

static void rvg_activate(pDevDesc dev) {
}


void rvg_SetLineSpec(pDevDesc dev, R_GE_gcontext *gc, int idx) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");

	float alpha =  R_ALPHA(gc->col)/255.0;


	if (gc->lty > -1 && gc->lwd > 0.0 && alpha > 0) {
		fprintf(pd->dmlFilePointer, " stroke=\"rgb(%d,%d,%d)\"", CREDC(gc->col), CGREENC(gc->col), CBLUEC(gc->col) );
		if( alpha < .9999 )
			fprintf(pd->dmlFilePointer, " stroke-opacity=\"%.3f\"", alpha );

		fprintf(pd->dmlFilePointer, " stroke-width=\"%.3f\"", gc->lwd );
		int newlty = gc->lty;
		double newlwd = gc->lwd;
		switch (gc->lty) {
		case LTY_BLANK:
			break;
		case LTY_SOLID:
			break;
		case LTY_DOTTED:
			fputs(" stroke-dasharray=\"1,5\"", pd->dmlFilePointer );
			break;
		case LTY_DASHED:
			fputs(" stroke-dasharray=\"5,5\"", pd->dmlFilePointer );
			break;
		case LTY_LONGDASH:
			fputs(" stroke-dasharray=\"10,5\"", pd->dmlFilePointer );
			break;
		case LTY_DOTDASH:
			fputs(" stroke-dasharray=\"1,5,5,5\"", pd->dmlFilePointer );
			break;
		case LTY_TWODASH:
			fputs(" stroke-dasharray=\"10,5,5,5\"", pd->dmlFilePointer );
			break;
		default:
			fputs(" stroke-dasharray=\"", pd->dmlFilePointer );
			for(int i=0 ; i<8 && newlty&15 ; i++) {
				int lwd = (int)newlwd * newlty;
				lwd = lwd & 15;
				if( i>0)
					fprintf(pd->dmlFilePointer, ",%i", lwd );
				else fprintf(pd->dmlFilePointer, "%i", lwd );
				newlty = newlty>>4;
			}
			fputs("\"", pd->dmlFilePointer );
			break;
		}

		switch (gc->ljoin) {
		case GE_ROUND_JOIN: //round
			fputs(" stroke-linejoin=\"round\"", pd->dmlFilePointer );
			break;
		case GE_MITRE_JOIN: //mitre
			fputs(" stroke-linejoin=\"miter\"", pd->dmlFilePointer );
			break;
		case GE_BEVEL_JOIN: //bevel
			fputs(" stroke-linejoin=\"bevel\"", pd->dmlFilePointer );
			break;
		default:
			fputs(" stroke-linejoin=\"round\"", pd->dmlFilePointer );
			break;
		}
	} else {
		fputs(" stroke-width=\"0\"", pd->dmlFilePointer );
	}
	setlocale(LC_NUMERIC, saved_locale);

}

void rvg_SetFillColor(pDevDesc dev, R_GE_gcontext *gc, int idx) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");

	float alpha =  R_ALPHA(gc->fill)/255.0;
	if (alpha > 0) {
		fprintf(pd->dmlFilePointer, " fill=\"rgb(%d,%d,%d)\"", CREDC(gc->fill), CGREENC(gc->fill), CBLUEC(gc->fill) );
		if( alpha < .9999 )
			fprintf(pd->dmlFilePointer, " fill-opacity=\"%.3f\"", alpha );
	} else fputs( " fill-opacity=\"0\"", pd->dmlFilePointer );
	setlocale(LC_NUMERIC, saved_locale);

}

void rvg_SetFontSpec(pDevDesc dev, R_GE_gcontext *gc) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char *saved_locale;
	saved_locale = setlocale(LC_NUMERIC, "C");

	float alpha =  R_ALPHA(gc->col)/255.0;
	double fontsize = gc->cex * gc->ps;


	if ( gc->cex > 0.0 && alpha > 0 ) {
		fputs(" style=\"", pd->dmlFilePointer);
		fprintf(pd->dmlFilePointer, " fill:rgb(%d,%d,%d);", CREDC(gc->col), CGREENC(gc->col), CBLUEC(gc->col) );
		if( alpha < .9999 )
			fprintf(pd->dmlFilePointer, "fill-opacity:%.3f;", alpha );
		fprintf(pd->dmlFilePointer, "font-family:%s;", pd->fi->fontname );
		fprintf(pd->dmlFilePointer, "font-size:%.0f;", fontsize );
		if (gc->fontface == 2) {
			fputs("font-weight:bold;", pd->dmlFilePointer );
		} else if (gc->fontface == 3) {
			fputs("font-style:italic;", pd->dmlFilePointer );
		} else if (gc->fontface == 4) {
			fputs("font-weight:bold;font-style:italic;", pd->dmlFilePointer );
		}
		fputs("\"", pd->dmlFilePointer );


	}

	setlocale(LC_NUMERIC, saved_locale);

}

static void rvg_Circle(double x, double y, double r, const pGEcontext gc,
		pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int idx = get_and_increment_idx(dev);
	register_element( dev);

	fprintf(pd->dmlFilePointer,
			"<circle id=\"elt_%d\" cx=\"%.5f\" cy=\"%.5f\" r=\"%.5f\"", idx, x, y, r);
	rvg_SetLineSpec(dev, gc, idx);
	rvg_SetFillColor(dev, gc, idx);
	fputs(" />\n", pd->dmlFilePointer);
	fflush(pd->dmlFilePointer);
}

static void rvg_Line(double x1, double y1, double x2, double y2,
		const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;

	DOC_ClipLine(x1, y1, x2, y2, dev);
	x1 = pd->clippedx0;y1 = pd->clippedy0;
	x2 = pd->clippedx1;y2 = pd->clippedy1;

	if (gc->lty > -1 && gc->lwd > 0.0 ){

		int idx = get_and_increment_idx(dev);
		register_element( dev);
		fprintf(pd->dmlFilePointer, "<line id=\"elt_%d\"", idx );
		fprintf(pd->dmlFilePointer, " x1=\"%.5f\" y1=\"%.5f\"", x1, y1);
		fprintf(pd->dmlFilePointer, " x2=\"%.5f\" y2=\"%.5f\"", x2, y2);

		rvg_SetLineSpec(dev, gc, idx);
		fputs(" />\n", pd->dmlFilePointer);
		fflush(pd->dmlFilePointer);
	}

}

static void rvg_Polyline(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int i;
	for (i = 1; i < n; i++) {
		DOC_ClipLine(x[i-1], y[i-1], x[i], y[i], dev);
		if( i < 2 ){
			x[i-1] = pd->clippedx0;
			y[i-1] = pd->clippedy0;
		}
		x[i] = pd->clippedx1;
		y[i] = pd->clippedy1;
	}

	if (gc->lty > -1 && gc->lwd > 0.0 ){

		int idx = get_and_increment_idx(dev);
		register_element( dev);
		int i;
		fprintf(pd->dmlFilePointer, "<polyline id=\"elt_%d\" points=", idx );
		fprintf(pd->dmlFilePointer, "\"%.5f,%.5f", x[0], y[0]);

		for (i = 1; i < n; i++) {
			fprintf(pd->dmlFilePointer, " %.5f,%.5f", x[i], y[i]);
		}
		fputs("\"", pd->dmlFilePointer );


		rvg_SetLineSpec(dev, gc, idx);
		fputs(" fill=\"none\"", pd->dmlFilePointer);
		fputs(" />\n", pd->dmlFilePointer);
		fflush(pd->dmlFilePointer);
	}
}

static void rvg_Polygon(int n, double *x, double *y, const pGEcontext gc,
		pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int i;
	for (i = 1; i < n; i++) {
		DOC_ClipLine(x[i-1], y[i-1], x[i], y[i], dev);
		if( i < 2 ){
			x[i-1] = pd->clippedx0;
			y[i-1] = pd->clippedy0;
		}
		x[i] = pd->clippedx1;
		y[i] = pd->clippedy1;
	}

	int idx = get_and_increment_idx(dev);
	register_element( dev);

	fprintf(pd->dmlFilePointer, "<polygon id=\"elt_%d\"", idx );
	fprintf(pd->dmlFilePointer, " points=\"%.5f,%.5f ", x[0], y[0]);

	for (i = 1; i < n; i++) {
		fprintf(pd->dmlFilePointer, " %.5f,%.5f", x[i], y[i]);
	}

	fputs("\"", pd->dmlFilePointer );

	rvg_SetLineSpec(dev, gc, idx);
	rvg_SetFillColor(dev, gc, idx);
	fputs(" />\n", pd->dmlFilePointer);

	fflush(pd->dmlFilePointer);
}

static void rvg_Rect(double x0, double y0, double x1, double y1,
		const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;

	double tmp;
	if (x0 >= x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	if (y0 >= y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	DOC_ClipRect(x0, y0, x1, y1, dev);
	x0 = pd->clippedx0;y0 = pd->clippedy0;
	x1 = pd->clippedx1;y1 = pd->clippedy1;

	int idx = get_and_increment_idx(dev);
	register_element( dev );


	fprintf(pd->dmlFilePointer, "<rect id=\"elt_%d\"", idx );
	fprintf(pd->dmlFilePointer, " x=\"%.5f\" y=\"%.5f\"", x0, y0);
	fprintf(pd->dmlFilePointer, " width=\"%.5f\" height=\"%.5f\"", x1-x0, y1-y0);

	rvg_SetLineSpec(dev, gc, idx);
	rvg_SetFillColor(dev, gc, idx);
	fputs(" />\n", pd->dmlFilePointer);

	fflush(pd->dmlFilePointer);

}
void rvg_text(const char *str, DOCDesc *pd){
    for( ; *str ; str++)
	switch(*str) {
	case '"':
		fprintf(pd->dmlFilePointer, "\\\"");
	    break;

	case '\n':
		fprintf(pd->dmlFilePointer, "\\\n");
	    break;

	default:
	    fputc(*str, pd->dmlFilePointer);
	    break;
	}
}

void rvg_textUTF8(const char *str, DOCDesc *pd){
	unsigned char *p;
	p = (unsigned char *) str;
	int val, val1, val2, val3, val4;
	while(*p){
		val = *(p++);
		if( val < 128 ){ /* ASCII */
			switch(val) {
				case '"':
					fprintf(pd->dmlFilePointer, "\\\"");
					break;

				case '\n':
					fprintf(pd->dmlFilePointer, "\\\n");
					break;

				default:
					fprintf(pd->dmlFilePointer, "%c", val);
					break;
			}

		} else if( val > 240 ){ /* 4 octets*/
			val1 = (val - 240) * 65536;
			val = *(p++);
			val2 = (val - 128) * 4096;
			val = *(p++);
			val3 = (val - 128) * 64;
			val = *(p++);
			val4 = val - 128;
			val = val1 + val2 + val3 + val4;

			char byte1 = 0xf0 | ((val & 0x1C0000) >> 16);
			char byte2 = 0x80 | ((val & 0x3F000)  >> 12);
			char byte3 = 0x80 | ((val & 0xFC0) >> 6);
			char byte4 = 0x80 | (val & 0x3f);
			fprintf(pd->dmlFilePointer, "%c%c%c%c", byte1, byte2, byte3, byte4);
		} else {
			if( val >= 224 ){ /* 3 octets : 224 = 128+64+32 */
				val1 = (val - 224) * 4096;
				val = *(p++);
				val2 = (val-128) * 64;
				val = *(p++);
				val3 = (val-128);
				val = val1 + val2 + val3;
				char byte1 = 0xe0 | ((val & 0xf000) >> 12);
				char byte2 = 0x80 | ((val & 0xfc0)  >> 6);
				char byte3 = 0x80 | (val & 0x3f);
				fprintf(pd->dmlFilePointer, "%c%c%c", byte1, byte2, byte3);
			} else { /* 2 octets : >192 = 128+64 */
				val1 = (val - 192) * 64;
				val = *(p++);
				val2 = val-128;
				val = val1 + val2;
				char byte1 = 0xc0 | ((val & 0x7c0) >> 6);
				char byte2 = 0x80 | (val & 0x3f);
				fprintf(pd->dmlFilePointer, "%c%c", byte1, byte2);
			}

		}
	}
}


static void rvg_Text(double x, double y, const char *str, double rot,
		double hadj, const pGEcontext gc, pDevDesc dev) {

	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	int idx = get_and_increment_idx(dev);
	register_element( dev);
	double w = rvg_StrWidth(str, gc, dev);
	//double h = getFontHeight(str, gc, dev);
	//Rprintf("TEXT:\t%s\n", str);
 	fprintf(pd->dmlFilePointer, "<text id=\"elt_%d\"", idx );
 	fprintf(pd->dmlFilePointer, " x=\"%.5f\" y=\"%.5f\"", x-hadj*w, y );

	fputs( " transform=\"", pd->dmlFilePointer );
	fprintf(pd->dmlFilePointer, "rotate(%.0f,%.5f,%.5f)", -rot, x, y );
	fputs("\"", pd->dmlFilePointer );

	rvg_SetFontSpec(dev, gc);
	fputs(" >\n\t", pd->dmlFilePointer );
	rvg_textUTF8(str, pd);
	fputs("\n\t</text>\n", pd->dmlFilePointer );


	//rvg_SetFontSpec(dev, gc, idx);


	fflush(pd->dmlFilePointer);

}

char* get_svg_filename(char* filename, int index){
	char *buf;
	int len = snprintf(NULL, 0, "%s_%03d.svg", filename,index);
	buf = (char*)malloc( (len + 1) * sizeof(char) );
	sprintf(buf, "%s_%03d.svg", filename,index);
	return buf;
}


static void rvg_NewPage(const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;

	pd->canvas_id++;

	if (pd->canvas_start_id < pd->canvas_id) {
		closeFile(pd->dmlFilePointer);
	}

	dev->right = pd->width;
	dev->bottom = pd->height;
	dev->left = 0;
	dev->top = 0;

	dev->clipLeft = 0;
	dev->clipRight = dev->right;
	dev->clipBottom = dev->bottom;
	dev->clipTop = 0;

	pd->clippedx0 = dev->clipLeft;
	pd->clippedy0 = dev->clipTop;
	pd->clippedx1 = dev->clipRight;
	pd->clippedy1 = dev->clipBottom;


	char *filename={0};
	filename = get_svg_filename(pd->filename, pd->canvas_id);

	pd->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, dev->right, dev->bottom);
	pd->cr = cairo_create (pd->surface);

	pd->dmlFilePointer = (FILE *) fopen(filename, "w");
	if (pd->dmlFilePointer == NULL) {
		Rf_error("error while opening %s\n", filename);
	}

	fprintf(pd->dmlFilePointer, "<svg height=\"%.0f\" width=\"%.0f\">\n",
			dev->right, dev->bottom);
	updateFontInfo(dev, gc);
	free(filename);
}

static void rvg_Close(pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	fputs("</svg>", pd->dmlFilePointer );
	closeFile(pd->dmlFilePointer);
	free(pd);
}

static void rvg_Clip(double x0, double x1, double y0, double y1, pDevDesc dev) {
	dev->clipLeft = x0;
	dev->clipRight = x1;
	dev->clipBottom = y1;
	dev->clipTop = y0;
}

static void rvg_MetricInfo(int c, const pGEcontext gc, double* ascent,
		double* descent, double* width, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	char str[16];

	char M[2];
	M[0]='M';
	M[1]='\0';

	int Unicode = mbcslocale;

	cairo_t *cc;
	cc = pd->cr;
	//Rprintf("\tMetricInfo\n" );
	if (!c) {
		//Rprintf("\tnon c\n");
		str[0]='M'; str[1]='g'; str[2]=0;
		/* this should give us a reasonably decent (g) and almost max width (M) */
	} else if (gc->fontface == 5) {
		char s[2];
		s[0] = c; s[1] = '\0';
		AdobeSymbol2utf8(str, s, 16);
		//Rprintf("\tfontface:5 & str:%s\n", str);

	} else if(Unicode) {
		Rf_ucstoutf8(str, (unsigned int) c);
		//Rprintf("\tUnicode & str:%s\n", str);
	} else {
		//Rprintf("normal\n");
		str[0] = c; str[1] = 0;
		//Rprintf("\tdefault & str:%s\n", str);
		/* Here, we assume that c < 256 */
	}



	cairo_text_extents_t te;
	cairo_text_extents (cc, str, &te);
	//Rprintf("\tcairo_text_extents ok \n");
	*ascent = -te.y_bearing;
	*descent = te.height+te.y_bearing;
	cairo_text_extents (cc, M, &te);
	*width = te.width;
}

static void rvg_Size(double *left, double *right, double *bottom, double *top,
		pDevDesc dev) {
	*left = dev->left;
	*right = dev->right;
	*bottom = dev->bottom;
	*top = dev->top;
}



static double rvg_StrWidth(const char *str, const pGEcontext gc, pDevDesc dev) {
	DOCDesc *pd = (DOCDesc *) dev->deviceSpecific;
	updateFontInfo(dev, gc);

	cairo_t *cc = pd->cr;
	cairo_text_extents_t te;
	cairo_text_extents(cc, str, &te);

	return te.x_advance;
}

/* Could support 'colormodel = "cmyk"' */
static void rvg_writeRaster(unsigned int *raster, int w, int h,
			   double x, double y,
			   double width, double height,
			   double rot,
			   Rboolean interpolate,
			   pDevDesc dd)
{
	DOCDesc *pd = (DOCDesc *) dd->deviceSpecific;
	Rprintf("w:%d, h:%d, x:%.2f, y:%.2f, width:%.2f, height:%.2f\n", w, h, x, y, width, height);

	double steph = height / h;
	double stepw = width / w;
	Rprintf("stepw:%.2f, steph:%.2f\n", stepw, steph);

	if( fabs(rot ) < 1 )
		fputs( "<g>", pd->dmlFilePointer);
	else{
		fputs( "<g ", pd->dmlFilePointer);
		fprintf(pd->dmlFilePointer, " transform=\"rotate(%.0f,0,0)\">\n", -rot );
	}

	int i = -1;
    for (int r = 0; r < h; r++){
    	for (int c = 0; c < w; c++){
    		i++;
    		fprintf(pd->dmlFilePointer, "\t<rect x=\"%.5f\" y=\"%.5f\"", x+(c*stepw), y-((r+1)*steph) );
    		fprintf(pd->dmlFilePointer, " width=\"%.5f\" height=\"%.5f\"", stepw, steph);

    		fputs(" stroke-width=\"0\"", pd->dmlFilePointer );
    		float alpha =  R_ALPHA(raster[i])/255.0;
    		if (alpha > 0) {
    			fprintf(pd->dmlFilePointer, " fill=\"rgb(%d,%d,%d)\"", CREDC(raster[i]), CGREENC(raster[i]), CBLUEC(raster[i]) );
    			if( alpha < .9999 )
    				fprintf(pd->dmlFilePointer, " fill-opacity=\"%.3f\"", alpha );
    		}
    		fputs(" />\n", pd->dmlFilePointer);
    	}
    }
    fputs( "\n</g>\n", pd->dmlFilePointer);
}

/* see comments above */
static void rvg_Raster(unsigned int *raster, int w, int h,
		      double x, double y,
		      double width, double height,
		      double rot,
		      Rboolean interpolate,
		      const pGEcontext gc, pDevDesc dev)
{
	if (height < 0)
		height = -height;

	int newW = (int) width;
	int newH = (int) height;

	if (interpolate) {
		unsigned int *newRaster =
				(unsigned int *) R_alloc(newW * newH, sizeof(unsigned int));

		R_GE_rasterInterpolate(raster, w, h,
				newRaster, newW, newH);
		rvg_writeRaster(newRaster, newW, newH,
				x, y, width, height, rot, FALSE, dev);
	} else {
		Rprintf("not interpolate\n");
		rvg_writeRaster(raster, w, h,
				x, y, newW, newH, rot, FALSE, dev);
	}

}


SEXP R_rvg_Device(SEXP filename, SEXP width, SEXP height, SEXP pointsize, SEXP fontfamily, SEXP canvas_id) {

	double w = asReal(width);
	double h = asReal(height);
	double ps = asReal(pointsize);

	int canvasid = INTEGER(canvas_id)[0];

	BEGIN_SUSPEND_INTERRUPTS;
	GE_rvgDevice(CHAR(STRING_ELT(filename, 0))
			, w, h, ps, CHAR(STRING_ELT(fontfamily, 0))
			, canvasid );
	END_SUSPEND_INTERRUPTS;
	return R_NilValue;
}


