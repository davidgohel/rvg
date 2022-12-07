#include "clipper.h"
#include "gdtools.h"
#include "R_ext/GraphicsEngine.h"

// SVG device metadata
class PPTX_dev {
public:
  FILE *file;
  std::string filename;
  int pageno;
  int id;
  double clipleft, clipright, cliptop, clipbottom;
  double offx;
  double offy;

  std::string raster_prefix;
  int img_last_id;

  Rcpp::List system_aliases;

  bool editable;
  int standalone;
  XPtrCairoContext cc;
  clipper *clp;

  PPTX_dev(std::string filename_,
           Rcpp::List& aliases_,
           bool editable_, double offx_, double offy_ , int id_,
           std::string raster_prefix_,
           int rel_last_id_, int standalone_,
           double width_, double height_ ):
    filename(filename_),
    pageno(0),
    id(id_),
    offx(offx_), offy(offy_),
    raster_prefix(raster_prefix_), img_last_id(rel_last_id_),
    system_aliases(Rcpp::wrap(aliases_["system"])),
    editable(editable_),
    standalone(standalone_),
    cc(gdtools::context_create()){

    file = fopen(R_ExpandFileName(filename.c_str()), "w");
    clipleft = 0.0;
    clipright = width_;
    cliptop = 0.0;
    clipbottom = height_;
    clp = new clipper();
  }

  bool ok() const {
    return file != NULL;
  }

  int new_id() {
    id++;
    return id;
  }
  int nex_id_rel() {
    img_last_id++;
    return img_last_id;
  }
  ~PPTX_dev() {
    if (ok())
      fclose(file);
  }
};

// SVG device metadata
class XLSX_dev {
public:
  FILE *file;
  std::string filename;
  int pageno;
  int id;
  double clipleft, clipright, cliptop, clipbottom;
  double offx;
  double offy;

  std::string raster_prefix;
  int img_last_id;

  Rcpp::List system_aliases;

  bool editable;
  int standalone;
  XPtrCairoContext cc;
  clipper *clp;

  XLSX_dev(std::string filename_,
           Rcpp::List& aliases_,
           bool editable_, double offx_, double offy_ , int id_,
           std::string raster_prefix_,
           int rel_last_id_, int standalone_,
           double width_, double height_ ):
    filename(filename_),
    pageno(0),
    id(id_),
    offx(offx_), offy(offy_),
    raster_prefix(raster_prefix_), img_last_id(rel_last_id_),
    system_aliases(Rcpp::wrap(aliases_["system"])),
    editable(editable_),
    standalone(standalone_),
    cc(gdtools::context_create()){

    file = fopen(R_ExpandFileName(filename.c_str()), "w");
    clipleft = 0.0;
    clipright = width_;
    cliptop = 0.0;
    clipbottom = height_;
    clp = new clipper();
  }

  bool ok() const {
    return file != NULL;
  }

  int new_id() {
    id++;
    return id;
  }
  int nex_id_rel() {
    img_last_id++;
    return img_last_id;
  }
  ~XLSX_dev() {
    if (ok())
      fclose(file);
  }
};
