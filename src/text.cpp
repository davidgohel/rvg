/*
 * DSVG device - Text handling
 */
#include "ooxml_dev.h"
#include "rpr.h"
#include "ppr.h"
#include "xfrm.h"
#include "Rcpp.h"
#include "a_prstgeom.h"
#include <systemfonts.h>
#include "write_nv_pr.h"
#include <math.h>
#include <iostream>
#include <Rcpp.h>

bool is_bold(int face) {
  return face == 2 || face == 4;
}
bool is_italic(int face) {
  return face == 3 || face == 4;
}

bool is_bolditalic(int face) {
  return face == 4;
}

bool is_symbol(int face) {
  return face == 5;
}





inline std::string find_alias_field(std::string& family, Rcpp::List& alias,
                                    const char* face, const char* field) {
  if (alias.containsElementNamed(face)) {
    Rcpp::List font = alias[face];
    if (font.containsElementNamed(field))
      return font[field];
  }
  return std::string();
}

inline std::string find_user_alias(std::string& family,
                                   Rcpp::List const& aliases,
                                   int face, const char* field) {
  std::string out;
  if (aliases.containsElementNamed(family.c_str())) {
    Rcpp::List alias = aliases[family];
    if (is_bolditalic(face))
      out = find_alias_field(family, alias, "bolditalic", field);
    else if (is_bold(face))
      out = find_alias_field(family, alias, "bold", field);
    else if (is_italic(face))
      out = find_alias_field(family, alias, "italic", field);
    else if (is_symbol(face))
      out = find_alias_field(family, alias, "symbol", field);
    else
      out = find_alias_field(family, alias, "plain", field);
  }
  return out;
}

inline std::string find_system_alias(std::string& family,
                                     Rcpp::List const& aliases) {
  std::string out;
  if (aliases.containsElementNamed(family.c_str())) {
    SEXP alias = aliases[family];
    if (TYPEOF(alias) == STRSXP && Rf_length(alias) == 1)
      out = Rcpp::as<std::string>(alias);
  }
  return out;
}

std::string fontname(const char* family_, int face,
                     Rcpp::List const& system_aliases) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  std::string alias = find_system_alias(family, system_aliases);

  if (alias.size())
    return alias;
  else
    return family;
}

std::string fontfile(const char* family_, int face,
                     Rcpp::List aliases) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  return find_user_alias(family, aliases, face, "file");
}

// Copied from the package svglite maintained by Thomas Lin Pedersen
FontSettings get_font_file(const char* family, int face) {
  const char* fontfamily = family;
  if (is_symbol(face)) {
    fontfamily = "symbol";
  } else if (strcmp(family, "") == 0) {
    fontfamily = "sans";
  }
  return locate_font_with_features(fontfamily, is_italic(face), is_bold(face));
}

void pptx_metric_info(int c, const pGEcontext gc, double* ascent,
                             double* descent, double* width, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  if (c < 0) {
    c = -c;
  }

  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);
  int error = glyph_metrics(c, font.file, font.index, gc->ps * gc->cex, 1e4, ascent, descent, width);
  if (error != 0) {
    *ascent = 0;
    *descent = 0;
    *width = 0;
  }

  double mod = 72./1e4;
  *ascent *= mod;
  *descent *= mod;
  *width *= mod;
}

void xlsx_metric_info(int c, const pGEcontext gc, double* ascent,
                             double* descent, double* width, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;

  if (c < 0) {
    c = -c;
  }

  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, xlsx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);
  int error = glyph_metrics(c, font.file, font.index, gc->ps * gc->cex, 1e4, ascent, descent, width);
  if (error != 0) {
    *ascent = 0;
    *descent = 0;
    *width = 0;
  }

  double mod = 72./1e4;
  *ascent *= mod;
  *descent *= mod;
  *width *= mod;
}

double pptx_strwidth_utf8(const char *str, const pGEcontext gc, pDevDesc dd) {

  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);
  double width = 0.0;
  int error = string_width(str, font.file, font.index, gc->ps * gc->cex, 1e4, 1, &width);
  if (error != 0) {
    width = 0.0;
  }
  return width * 72. / 1e4;
}

double xlsx_strwidth_utf8(const char *str, const pGEcontext gc, pDevDesc dd) {

  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, xlsx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);
  double width = 0.0;
  int error = string_width(str, font.file, font.index, gc->ps * gc->cex, 1e4, 1, &width);
  if (error != 0) {
    width = 0.0;
  }
  return width * 72. / 1e4;
}


double pptx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  return pptx_strwidth_utf8(Rf_translateCharUTF8(Rf_mkChar(str)), gc, dd);
}


double xlsx_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  return xlsx_strwidth_utf8(Rf_translateCharUTF8(Rf_mkChar(str)), gc, dd);
}


double pptx_strheight_utf8(const char *str, const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);

  // Use 'M' character to get font height
  double ascent = 0.0, descent = 0.0, width = 0.0;
  int error = glyph_metrics(77, font.file, font.index, gc->ps * gc->cex, 1e4, &ascent, &descent, &width);
  if (error != 0) {
    return 0.0;
  }
  return (ascent + descent) * 72. / 1e4;
}
double xlsx_strheight_utf8(const char *str, const pGEcontext gc, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, xlsx_obj->system_aliases);
  FontSettings font = get_font_file(fontname_.c_str(), gc->fontface);

  // Use 'M' character to get font height
  double ascent = 0.0, descent = 0.0, width = 0.0;
  int error = glyph_metrics(77, font.file, font.index, gc->ps * gc->cex, 1e4, &ascent, &descent, &width);
  if (error != 0) {
    return 0.0;
  }
  return (ascent + descent) * 72. / 1e4;
}
double translate_rotate_x(double x, double y, double rot, double h, double w, double hadj) {
  double pi = 3.141592653589793115997963468544185161590576171875;
  double alpha = -rot * pi / 180;

  double Px = x + (0.5-hadj) * w;
  double Py = y - 0.5 * h;

  double _cos = cos( alpha );
  double _sin = sin( alpha );

  double Ppx = x + (Px-x) * _cos - (Py-y) * _sin ;

  return Ppx - 0.5 * w;
}


double translate_rotate_y(double x, double y, double rot, double h, double w, double hadj) {
  double pi = 3.141592653589793115997963468544185161590576171875;
  double alpha = -rot * pi / 180;

  double Px = x + (0.5-hadj) * w;
  double Py = y - 0.5 * h;

  double _cos = cos( alpha );
  double _sin = sin( alpha );

  double Ppy = y + (Px-x) * _sin + (Py-y) * _cos;

  return Ppy - 0.5 * h;
}

void write_t_pptx(pDevDesc dev, const char* text) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dev->deviceSpecific;
  fputs("<a:t>", pptx_obj->file );

  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", pptx_obj->file); break;
    case '<': fputs("&lt;",  pptx_obj->file); break;
    case '>': fputs("&gt;",  pptx_obj->file); break;
    default:  fputc(*cur,    pptx_obj->file);
    }
  }
  fputs("</a:t>", pptx_obj->file );
}


std::string pptx_body_pr()
{
  std::stringstream os;

  os << "<a:bodyPr ";
  os << "lIns=\"0\" rIns=\"0\" tIns=\"0\" bIns=\"0\" ";
  os << "anchorCtr=\"1\" anchor=\"ctr\" wrap=\"none\"/>";
  os << "<a:lstStyle/>";

  return os.str();
}


void write_text_body_pptx(pDevDesc dd, R_GE_gcontext *gc, const char* text, double hadj, double fontsize, double fontheight) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dd->deviceSpecific;
  ppr a_ppr_(hadj, fontsize);
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, pptx_obj->system_aliases);

  rpr rpr_(fontsize, is_italic(gc->fontface), is_bold(gc->fontface), gc->col, fontname_);

  fputs("<p:txBody>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", pptx_body_pr().c_str() );
  fputs("<a:p>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", a_ppr_.a_tag().c_str() );
  fputs("<a:r>", pptx_obj->file );
  fprintf(pptx_obj->file, "%s", rpr_.a_tag().c_str() );
  write_t_pptx(dd, text);
  fputs("</a:r>", pptx_obj->file );
  fputs("</a:p>", pptx_obj->file );
  fputs("</p:txBody>", pptx_obj->file );
}

void pptx_text_utf8(double x, double y, const char *str, double rot,
                           double hadj, const pGEcontext gc, pDevDesc dd) {
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;

  double fs = gc->cex * gc->ps ;
  double w = pptx_strwidth_utf8(str, gc, dd);
  double h = pptx_strheight_utf8(str, gc, dd);
  if( fs*100 < 1.0 ) return;

  double corrected_offx = translate_rotate_x(x, y, rot, h, w, hadj) ;
  double corrected_offy = translate_rotate_y(x, y, rot, h, w, hadj) ;

  xfrm xfrm_(pptx_obj->offx + corrected_offx, pptx_obj->offy + corrected_offy, w, h, -rot );


  fputs("<p:sp>", pptx_obj->file);
  write_nv_pr_pptx(dd, "tx");
  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  fputs("<a:noFill/>", pptx_obj->file);
  fputs("</p:spPr>", pptx_obj->file);

  write_text_body_pptx(dd, gc, str, hadj, fs, h);
  fputs("</p:sp>", pptx_obj->file);
}

void pptx_text(double x, double y, const char *str, double rot,
                      double hadj, const pGEcontext gc, pDevDesc dd) {

  return pptx_text_utf8(x, y, Rf_translateCharUTF8(Rf_mkChar(str)), rot, hadj, gc, dd);
}


std::string xlsx_body_pr()
{
  std::stringstream os;

  os << "<a:bodyPr ";
  os << "lIns=\"0\" rIns=\"0\" tIns=\"0\" bIns=\"0\" ";
  os << "anchorCtr=\"1\" anchor=\"ctr\" wrap=\"none\"/>";
  os << "<a:lstStyle/>";

  return os.str();
}


void write_t_xlsx(pDevDesc dev, const char* text) {
  XLSX_dev *xlsx_obj = (XLSX_dev *) dev->deviceSpecific;
  fputs("<a:t>", xlsx_obj->file );

  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fputs("&amp;", xlsx_obj->file); break;
    case '<': fputs("&lt;",  xlsx_obj->file); break;
    case '>': fputs("&gt;",  xlsx_obj->file); break;
    default:  fputc(*cur,    xlsx_obj->file);
    }
  }
  fputs("</a:t>", xlsx_obj->file );
}


void write_text_body_xlsx(pDevDesc dd, R_GE_gcontext *gc, const char* text, double hadj, double fontsize, double fontheight) {
  XLSX_dev *xlsx_obj = (XLSX_dev *) dd->deviceSpecific;
  ppr a_ppr_(hadj, fontsize);
  std::string fontname_ = fontname(gc->fontfamily, gc->fontface, xlsx_obj->system_aliases);

  rpr rpr_(fontsize, is_italic(gc->fontface), is_bold(gc->fontface), gc->col, fontname_);

  fputs("<xdr:txBody>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", xlsx_body_pr().c_str() );
  fputs("<a:p>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", a_ppr_.a_tag().c_str() );
  fputs("<a:r>", xlsx_obj->file );
  fprintf(xlsx_obj->file, "%s", rpr_.a_tag().c_str() );
  write_t_xlsx(dd, text);
  fputs("</a:r>", xlsx_obj->file );
  fputs("</a:p>", xlsx_obj->file );
  fputs("</xdr:txBody>", xlsx_obj->file );
}
void xlsx_text_utf8(double x, double y, const char *str, double rot,
                           double hadj, const pGEcontext gc, pDevDesc dd) {
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;

  double fs = gc->cex * gc->ps ;
  double w = xlsx_strwidth_utf8(str, gc, dd);
  double h = xlsx_strheight_utf8(str, gc, dd);
  if( fs*100 < 1.0 ) return;

  double corrected_offx = translate_rotate_x(x, y, rot, h, w, hadj) ;
  double corrected_offy = translate_rotate_y(x, y, rot, h, w, hadj) ;

  xfrm xfrm_(xlsx_obj->offx + corrected_offx, xlsx_obj->offy + corrected_offy, w, h, -rot );


  fputs("<xdr:sp>", xlsx_obj->file);
  write_nv_pr_xlsx(dd, "tx");
  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(xlsx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  fputs("<a:noFill/>", xlsx_obj->file);
  fputs("</xdr:spPr>", xlsx_obj->file);

  write_text_body_xlsx(dd, gc, str, hadj, fs, h);
  fputs("</xdr:sp>", xlsx_obj->file);
}

void xlsx_text(double x, double y, const char *str, double rot,
                      double hadj, const pGEcontext gc, pDevDesc dd) {
  return xlsx_text_utf8(x, y, Rf_translateCharUTF8(Rf_mkChar(str)), rot, hadj, gc, dd);
}
