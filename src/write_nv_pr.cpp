#include "ooxml_dev.h"
#include <iostream>
#include <string.h>

char* x_tag(int id, int editable, std::string label)
{
  std::stringstream os;
  os << "<xdr:nvSpPr>";
  os << "<xdr:cNvPr id=\"" << id << "\" name=\"" << label << id << "\"/>";
  if( editable < 1 ){
    os << "<xdr:cNvSpPr>";
    os << "<a:spLocks noSelect=\"1\" noResize=\"1\" noEditPoints=\"1\" noTextEdit=\"1\" noMove=\"1\" noRot=\"1\" noChangeShapeType=\"1\"/>";
    os << "</xdr:cNvSpPr>" ;
  } else {
    os << "<xdr:cNvSpPr/>";
  }
  os << "<xdr:nvPr/>";
  os << "</xdr:nvSpPr>";

  std::string s = os.str();
  char* str = new char[s.length() + 1];
  std::copy(s.c_str(), s.c_str() + s.length() + 1, str);

  return str;
}

void write_nv_pr_xlsx(pDevDesc dev, const char *label) {
  XLSX_dev *xlsx_obj = (XLSX_dev *) dev->deviceSpecific;
  int idx = xlsx_obj->new_id();
  fprintf(xlsx_obj->file, "%s", x_tag(idx, (int)(xlsx_obj->editable), label));
}

char* p_tag(int id, int editable, std::string label)
{
  std::stringstream os;
  os << "<p:nvSpPr>";
  os << "<p:cNvPr id=\"" << id << "\" name=\"" << label << id << "\"/>";
  if( editable < 1 ){
    os << "<p:cNvSpPr>";
    os << "<a:spLocks noSelect=\"1\" noResize=\"1\" noEditPoints=\"1\" noTextEdit=\"1\" noMove=\"1\" noRot=\"1\" noChangeShapeType=\"1\"/>";
    os << "</p:cNvSpPr>" ;
  } else {
    os << "<p:cNvSpPr/>";
  }
  os << "<p:nvPr/>";
  os << "</p:nvSpPr>";
  std::string s = os.str();
  char* str = new char[s.length() + 1];
  std::copy(s.c_str(), s.c_str() + s.length() + 1, str);

  return str;
}

void write_nv_pr_pptx(pDevDesc dev, const char *label) {
  PPTX_dev *pptx_obj = (PPTX_dev *) dev->deviceSpecific;
  int idx = pptx_obj->new_id();
  fprintf(pptx_obj->file, "%s", p_tag(idx, (int)(pptx_obj->editable), label));
}
