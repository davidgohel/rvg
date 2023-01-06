/*
 * RVG device - Raster handling
 */

extern "C" {
#include <png.h>
}
#include <cstdint>
#include "Rcpp.h"
#include "ooxml_dev.h"
#include "xfrm.h"
#include "a_prstgeom.h"

// This code has been copied from the package svglite maintained by Thomas Lin Pedersen
void raster_to_file(unsigned int *raster, int w, int h, double width, double height, bool interpolate, char *file_name) {
  h = h < 0 ? -h : h;
  w = w < 0 ? -w : w;
  bool resize = false;
  int w_fac = 1, h_fac = 1;
  std::vector<unsigned int> raster_resize;

  if (!interpolate && double(w) < width) {
    resize = true;
    w_fac = std::ceil(width / w);
  }
  if (!interpolate && double(h) < height) {
    resize = true;
    h_fac = std::ceil(height / h);
  }

  if (resize) {
    int w_new = w * w_fac;
    int h_new = h * h_fac;
    raster_resize.reserve(w_new * h_new);
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        unsigned int val = raster[i * w + j];
        for (int wrep = 0; wrep < w_fac; ++wrep) {
          raster_resize.push_back(val);
        }
      }
      for (int hrep = 1; hrep < h_fac; ++hrep) {
        raster_resize.insert(raster_resize.end(), raster_resize.end() - w_new, raster_resize.end());
      }
    }
    raster = raster_resize.data();
    w = w_new;
    h = h_new;
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    return;
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    png_destroy_write_struct(&png, (png_infopp)NULL);
    return;
  }
  if (setjmp(png_jmpbuf(png))) {
    png_destroy_write_struct(&png, &info);
    return;
  }
  png_set_IHDR(
    png,
    info,
    w, h,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  std::vector<uint8_t*> rows(h);
  for (int y = 0; y < h; ++y) {
    rows[y] = (uint8_t*)raster + y * w * 4;
  }

  std::vector<std::uint8_t> buffer;

  FILE *fp = fopen(file_name, "wb");
  png_init_io(png, fp);
  png_set_rows(png, info, &rows[0]);
  png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
  png_destroy_write_struct(&png, &info);

  fclose(fp);
}

void pptx_raster(unsigned int *raster, int w, int h,
                 double x, double y,
                 double width, double height,
                 double rot,
                 Rboolean interpolate,
                 const pGEcontext gc, pDevDesc dd)
{
  PPTX_dev *pptx_obj = (PPTX_dev*) dd->deviceSpecific;
  std::stringstream os;
  int idx = pptx_obj->new_id();
  int id_img_rel = pptx_obj->nex_id_rel();

  os << pptx_obj->raster_prefix << "rId";
  os.fill('0');
  os.width(6);
  os << id_img_rel;
  os << ".png";
  std::string s = os.str();
  char* fil = new char[s.length() + 1];
  std::copy(s.c_str(), s.c_str() + s.length() + 1, fil);


  if (height < 0)
    height = -height;
  xfrm xfrm_(pptx_obj->offx + x, pptx_obj->offy + y - height, width, height, -rot );

  raster_to_file(raster, w, h, width, height, interpolate, fil);

  fputs("<p:pic>", pptx_obj->file);
  fputs("<p:nvPicPr>", pptx_obj->file);
  fprintf(pptx_obj->file,
          "<p:cNvPr id=\"%d\" name=\"pic%d\"/>",
            idx, idx );
  fputs("<p:cNvPicPr/>", pptx_obj->file);
  fputs("<p:nvPr/>", pptx_obj->file);
  fputs("</p:nvPicPr>", pptx_obj->file);
  fputs("<p:blipFill>", pptx_obj->file);
  fprintf(pptx_obj->file,
          "<a:blip r:embed=\"%s\" cstate=\"print\"/>",
          fil);
  fputs("<a:stretch><a:fillRect/></a:stretch>", pptx_obj->file);
  fputs("</p:blipFill>", pptx_obj->file);

  fputs("<p:spPr>", pptx_obj->file);
  fprintf(pptx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(pptx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  fputs("</p:spPr>", pptx_obj->file);
  fputs("</p:pic>", pptx_obj->file);
}

void xlsx_raster(unsigned int *raster, int w, int h,
                 double x, double y,
                 double width, double height,
                 double rot,
                 Rboolean interpolate,
                 const pGEcontext gc, pDevDesc dd)
{
  XLSX_dev *xlsx_obj = (XLSX_dev*) dd->deviceSpecific;
  std::stringstream os;
  int idx = xlsx_obj->new_id();
  int id_img_rel = xlsx_obj->nex_id_rel();

  os << xlsx_obj->raster_prefix << "rId";
  os.fill('0');
  os.width(6);
  os << id_img_rel;
  os << ".png";
  std::string s = os.str();
  char* fil = new char[s.length() + 1];
  std::copy(s.c_str(), s.c_str() + s.length() + 1, fil);


  if (height < 0)
    height = -height;
  xfrm xfrm_(xlsx_obj->offx + x, xlsx_obj->offy + y - height, width, height, -rot );

  raster_to_file(raster, w, h, width, height, interpolate, fil);
  fputs("<xdr:pic>", xlsx_obj->file);
  fputs("<xdr:nvPicPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file,
          "<xdr:cNvPr id=\"%d\" name=\"pic%d\"/>",
          idx, idx );
  fputs("<xdr:cNvPicPr/>", xlsx_obj->file);
  fputs("<xdr:nvPr/>", xlsx_obj->file);
  fputs("</xdr:nvPicPr>", xlsx_obj->file);
  fputs("<xdr:blipFill>", xlsx_obj->file);
  fprintf(xlsx_obj->file,
          "<a:blip r:embed=\"rId%d\" cstate=\"print\"/>",
          id_img_rel);
  fputs("<a:stretch><a:fillRect/></a:stretch>", xlsx_obj->file);
  fputs("</xdr:blipFill>", xlsx_obj->file);

  fputs("<xdr:spPr>", xlsx_obj->file);
  fprintf(xlsx_obj->file, "%s", xfrm_.xml().c_str());
  fprintf(xlsx_obj->file,"%s", a_prstgeom::a_tag("rect").c_str());
  fputs("</xdr:spPr>", xlsx_obj->file);
  fputs("</xdr:pic>", xlsx_obj->file);
}
