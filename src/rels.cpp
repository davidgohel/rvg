#include <Rcpp.h>
#include "rels.h"
#include <iostream>

using namespace Rcpp;

std::string rels::xml()
{
  std::stringstream os;

  os << "<Relationships>";
  for( int i = 0 ; i < this->relationships.size() ; i++ ){
    os << this->relationships[i].xml();
  }
  os << "</Relationships>";
  return os.str();
}

void rels::add(relationship rel)
{
  this->relationships.push_back(rel);
}
int rels::next_id()
{
  int max = 0;
  int temp_id = 0;
  for( int i = 0 ; i < this->relationships.size() ; i++ ){
    temp_id = this->relationships[i].get_id();
    if( temp_id > max )
      max = temp_id;
  }
  return max+1;
}

rels::rels (std::vector<int> id_, std::vector<std::string> type_, std::vector<std::string> target_ ){
  int rels_n = id_.size();

  for(int i = 0 ; i < rels_n ; i++ ){
    relationship current_rel( id_[i], type_[i], target_[i] );
    this->relationships.push_back(current_rel);
  }
}
