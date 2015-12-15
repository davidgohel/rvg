#include <Rcpp.h>
#include "relationship.h"
#include <iostream>
#include "R_ext/GraphicsEngine.h"

using namespace Rcpp;

std::string relationship::xml()
{

  std::stringstream os;

  os << "<Relationship ";
  os << "Id=\"rId" << this->id << "\" ";
  os << "Type=\"" << this->type << "\" ";
  os << "Target=\"" << this->target << "\"";
  os << "></Relationship>";
  return os.str();
}

int relationship::get_id()
{
  return this->id;
}

relationship::relationship (int id_, std::string type_, std::string target_ ):
  id(id_),
  type(type_),
  target(target_){

}
