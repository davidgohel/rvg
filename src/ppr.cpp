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
  os << "<a:lnSpc><a:spcPts val=\"" << (int)(this->lineheight*100) << "\"/></a:lnSpc>";
  os << "<a:spcAft><a:spcPts val=\"0\"/></a:spcAft>";
  os << "<a:spcBef><a:spcPts val=\"0\"/></a:spcBef>";
  os << "</a:pPr>";
  return os.str();
}

std::string ppr::w_tag()
{

  std::stringstream os;
  os << "<w:pPr>";
//   if (this->hadj < 0.25)
//     os << "<w:jc w:val=\"left\"/>";
  // else if (hadj < 0.75)
    os << "<w:jc w:val=\"center\"/>";
//   else
//     os << "<w:jc w:val=\"right\"/>";

  // os << "<w:spacing w:after=\"0\" w:before=\"0\" w:line=\"" << (int)(this->lineheight*20) << "\" w:lineRule=\"exact\"/>";
  os << "<w:spacing w:after=\"0\" w:lineRule=\"auto\"/>";
  os << "<w:ind w:left=\"0\" w:right=\"0\" w:firstLine=\"0\" w:hanging=\"0\"/>";
  //write_w_rpr(dev, gc, h);
  os << "</w:pPr>";

  return os.str();
}

ppr::ppr (double hadj, double lineheight ):
  hadj(hadj), lineheight(lineheight){

}
