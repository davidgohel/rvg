/*
 * RVG device - Raster handling
 */
#ifndef RVG_RASTER_INCLUDED
#define RVG_RASTER_INCLUDED

void pptx_raster(unsigned int *raster, int w, int h,
                 double x, double y,
                 double width, double height,
                 double rot,
                 Rboolean interpolate,
                 const pGEcontext gc, pDevDesc dd);
void xlsx_raster(unsigned int *raster, int w, int h,
                 double x, double y,
                 double width, double height,
                 double rot,
                 Rboolean interpolate,
                 const pGEcontext gc, pDevDesc dd);

#endif // RVG_RASTER_INCLUDED
