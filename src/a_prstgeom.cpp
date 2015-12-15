#include <Rcpp.h>
#include "a_prstgeom.h"
#include <iostream>

using namespace Rcpp;

std::string a_prstgeom::a_tag(std::string preset_geom)
{
  std::stringstream os;

  os << "<a:prstGeom ";
  os << "prst=\""<< preset_geom << "\">";
  os << "<a:avLst/>";
  os << "</a:prstGeom>";
  return os.str();
}
