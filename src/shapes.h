/*
 * PPTX device - Base PPTX shapes handling
 */
#ifndef PPTX_SHAPES_INCLUDED
#define PPTX_SHAPES_INCLUDED

void pptx_line(double x1, double y1, double x2, double y2,
               const pGEcontext gc, pDevDesc dd);

void pptx_polyline(int n, double *x, double *y,
                   const pGEcontext gc, pDevDesc dd);

void pptx_polygon(int n, double *x, double *y,
                  const pGEcontext gc, pDevDesc dd);

void pptx_path(double *x, double *y, int npoly, int *nper, Rboolean winding,
               const pGEcontext gc, pDevDesc dd);

void pptx_rect(double x0, double y0, double x1, double y1,
               const pGEcontext gc, pDevDesc dd);

void pptx_circle(double x, double y, double r,
                 const pGEcontext gc, pDevDesc dd);

void xlsx_line(double x1, double y1, double x2, double y2,
               const pGEcontext gc, pDevDesc dd);

void xlsx_polyline(int n, double *x, double *y,
                   const pGEcontext gc, pDevDesc dd);

void xlsx_polygon(int n, double *x, double *y,
                  const pGEcontext gc, pDevDesc dd);

void xlsx_path(double *x, double *y, int npoly, int *nper, Rboolean winding,
               const pGEcontext gc, pDevDesc dd);

void xlsx_rect(double x0, double y0, double x1, double y1,
               const pGEcontext gc, pDevDesc dd);

void xlsx_circle(double x, double y, double r,
                 const pGEcontext gc, pDevDesc dd);

#endif // PPTX_SHAPES_INCLUDED
