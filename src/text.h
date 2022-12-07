/*
 * DSVG device - Text handling
 */
#ifndef RVG_TEXT_INCLUDED
#define RVG_TEXT_INCLUDED

void pptx_metric_info(int c, const pGEcontext gc, double* ascent,
                      double* descent, double* width, pDevDesc dd);

double pptx_strwidth_utf8(const char *str, const pGEcontext gc, pDevDesc dd);

double pptx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd);

void pptx_text_utf8(double x, double y, const char *str, double rot,
                    double hadj, const pGEcontext gc, pDevDesc dd);

void pptx_text(double x, double y, const char *str, double rot,
               double hadj, const pGEcontext gc, pDevDesc dd);

void xlsx_metric_info(int c, const pGEcontext gc, double* ascent,
                      double* descent, double* width, pDevDesc dd);

double xlsx_strwidth_utf8(const char *str, const pGEcontext gc, pDevDesc dd);

double xlsx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd);

void xlsx_text_utf8(double x, double y, const char *str, double rot,
                    double hadj, const pGEcontext gc, pDevDesc dd);

void xlsx_text(double x, double y, const char *str, double rot,
               double hadj, const pGEcontext gc, pDevDesc dd);

#endif // RVG_TEXT_INCLUDED
