#include <Rcpp.h>
#include "ppr.h"
#include <iostream>

using namespace Rcpp;

std::string ppr::a_tag()
{
  std::stringstream os;
  os << "<a:pPr";

  if (this->hadj < 0.25)
    os << " algn=\"l\"";
  else if (hadj < 0.75)
    os << " algn=\"ctr\"";
  else
    os << " algn=\"r\"";
  os << " marL=\"0\" marR=\"0\" indent=\"0\">";
  os << "<a:lnSpc><a:spcPct val=\"100%\"/></a:lnSpc>";
  os << "<a:spcBef><a:spcPts val=\"0\"/></a:spcBef>";
  os << "<a:spcAft><a:spcPts val=\"0\"/></a:spcAft>";
  os << "</a:pPr>";
  return os.str();
}


ppr::ppr (double hadj, double lineheight ):
  hadj(hadj), lineheight(lineheight){

}
