#include <Rcpp.h>
#include "main_tree.h"
#include <iostream>

using namespace Rcpp;

std::string main_tree::a_opening_tag()
{
  std::stringstream os;

  if( this->standalone > 0 ){
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    os << "<p:spTree xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" ";
    os << "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ";
    os << "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\" ";
    os << "xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\" >";


    os << "<p:nvGrpSpPr>";
    os << "<p:cNvPr id=\"" << this->id << "\" name=\"plot" << this->id << "\"/>";
    os << "<p:cNvGrpSpPr><a:grpSpLocks noResize=\"1\" noUngrp=\"1\" noChangeAspect=\"1\"/></p:cNvGrpSpPr>";
    os << "<p:nvPr/>";
    os << "</p:nvGrpSpPr>";
    os << "<p:grpSpPr>";
    os << "<a:xfrm>";
    os << "<a:off x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
    os << "<a:ext cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
    os << "<a:chOff x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
    os << "<a:chExt cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
    os << "</a:xfrm>";
    os << "</p:grpSpPr>";
    os << "<p:grpSp>";

  } else {
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    os << "<p:grpSp xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" " <<
      "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" " <<
        "xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\" " <<
          "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">";
  }



  os << "<p:nvGrpSpPr>";
  os << "<p:cNvPr id=\"" << this->group_id << "\" name=\"grp" << this->group_id << "\"/>";
  os << "<p:cNvGrpSpPr/>";
  os << "<p:nvPr/>";
  os << "</p:nvGrpSpPr>";

  os << "<p:grpSpPr>";
  os << "<a:xfrm>";
  os << "<a:off x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
  os << "<a:ext cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
  os << "<a:chOff x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
  os << "<a:chExt cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
  os << "</a:xfrm>";
  os << "</p:grpSpPr>";

  return os.str();
}

std::string main_tree::a_closing_tag(int standalone)
{
  std::stringstream os;

  if( standalone > 0 ){
    os << "</p:grpSp>";
    os << "</p:spTree>";
  } else os << "</p:grpSp>";

  return os.str();
}


std::string main_tree::x_opening_tag()
{
  std::stringstream os;

  if( this->standalone > 0 ){
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    os << "<xdr:wsDr xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" ";
    os << "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ";
    os << "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\" ";
    os << "xmlns:xdr=\"http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing\" >";
  } else {
    os << "<xdr:wsDr>";
  }

  os << "<xdr:absoluteAnchor>";
    os << "<xdr:pos x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
    os << "<xdr:ext cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";

    os << "<xdr:grpSp>";

    os << "<xdr:nvGrpSpPr>";
    os << "<xdr:cNvPr id=\"" << this->group_id << "\" name=\"grp" << this->group_id << "\"/>";
    os << "<xdr:cNvGrpSpPr/>";
    os << "<xdr:nvPr/>";
    os << "</xdr:nvGrpSpPr>";

    os << "<xdr:grpSpPr>";
    os << "<a:xfrm>";
    os << "<a:off x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
    os << "<a:ext cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
    os << "<a:chOff x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
    os << "<a:chExt cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
    os << "</a:xfrm>";
    os << "</xdr:grpSpPr>";

    return os.str();
}

std::string main_tree::x_closing_tag()
{
  std::stringstream os;
  os << "</xdr:grpSp>";
  os << "<xdr:clientData/>";
  os << "</xdr:absoluteAnchor>";
  os << "</xdr:wsDr>";

  return os.str();
}

main_tree::main_tree (int id, int group_id,
                      double offx, double offy,
                      double width, double height, int standalone):
  id(id),
  group_id(group_id),
  offx(offx),
  offy(offy),
  width(width),
  height(height), standalone(standalone) {

}
