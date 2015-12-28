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
  } else {
    os << "<p:spTree>";
  }

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

std::string main_tree::a_closing_tag()
{
  std::stringstream os;

  os << "</p:grpSp>";
  os << "</p:spTree>";

  return os.str();
}


std::string main_tree::w_opening_tag()
{

  std::stringstream os;
  if( this->standalone > 0 ){
    // os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    os << "<w:drawing xmlns:wpc=\"http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas\" ";
    os << "xmlns:mc=\"http://schemas.openxmlformats.org/markup-compatibility/2006\" ";
    os << "xmlns:o=\"urn:schemas-microsoft-com:office:office\" ";
    os << "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ";
    os << "xmlns:m=\"http://schemas.openxmlformats.org/officeDocument/2006/math\" ";
    os << "xmlns:v=\"urn:schemas-microsoft-com:vml\" ";
    os << "xmlns:wp14=\"http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing\" ";
    os << "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" ";
    os << "xmlns:w10=\"urn:schemas-microsoft-com:office:word\" ";
    os << "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" ";
    os << "xmlns:w14=\"http://schemas.microsoft.com/office/word/2010/wordml\" ";
    os << "xmlns:w15=\"http://schemas.microsoft.com/office/word/2012/wordml\" ";
    os << "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\" ";
    os << "xmlns:wpi=\"http://schemas.microsoft.com/office/word/2010/wordprocessingInk\" ";
    os << "xmlns:wne=\"http://schemas.microsoft.com/office/word/2006/wordml\" ";
    os << "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" ";
    os << "mc:Ignorable=\"w14 w15 wp14\" >";
  } else {
    os << "<w:drawing>";
  }

  os << "<wp:inline distT=\"0\" distB=\"0\" distL=\"0\" distR=\"0\">";
  os << "<wp:extent cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
  os << "<wp:docPr id=\"" << this->id << "\" name=\"plot" << this->id << "\"/>";
  os << "<wp:cNvGraphicFramePr/>";


  os << "<a:graphic xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\">";
  os << "<a:graphicData uri=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\">";
  os << "<wpg:wgp>";
  os << "<wpg:cNvGrpSpPr/>";
  os << "<wpg:grpSpPr>";

  os << "<a:xfrm>";
  os << "<a:off x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
  os << "<a:ext cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
  os << "<a:chOff x=\"" << (int)(this->offx * 12700) << "\" y=\"" << (int)(this->offy * 12700) << "\"/>";
  os << "<a:chExt cx=\"" << (int)(this->width * 12700) << "\" cy=\"" << (int)(this->height * 12700) << "\"/>";
  os << "</a:xfrm>";

  os << "</wpg:grpSpPr>";

  return os.str();
}
std::string main_tree::w_closing_tag()
{
  std::stringstream os;

  os << "</wpg:wgp>";
  os << "</a:graphicData>";
  os << "</a:graphic>";
  os << "</wp:inline>";
  os << "</w:drawing>";

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
