#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME:
Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP rvg_add_attribute(SEXP, SEXP, SEXP, SEXP);
extern SEXP rvg_collect_id(SEXP);
extern SEXP rvg_DOCX_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP rvg_DSVG_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP rvg_PPTX_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP rvg_set_tracer_off(SEXP);
extern SEXP rvg_set_tracer_on(SEXP);
extern SEXP rvg_XLSX_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"rvg_add_attribute",  (DL_FUNC) &rvg_add_attribute,   4},
  {"rvg_collect_id",     (DL_FUNC) &rvg_collect_id,      1},
  {"rvg_DOCX_",          (DL_FUNC) &rvg_DOCX_,          11},
  {"rvg_DSVG_",          (DL_FUNC) &rvg_DSVG_,           8},
  {"rvg_PPTX_",          (DL_FUNC) &rvg_PPTX_,          13},
  {"rvg_set_tracer_off", (DL_FUNC) &rvg_set_tracer_off,  1},
  {"rvg_set_tracer_on",  (DL_FUNC) &rvg_set_tracer_on,   1},
  {"rvg_XLSX_",          (DL_FUNC) &rvg_XLSX_,          13},
  {NULL, NULL, 0}
};

void R_init_rvg(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
