#include "relationship.h"

class rels
{
public:
  rels ( std::vector<int>, std::vector<std::string>, std::vector<std::string> );
  rels ( );
  std::string xml();
  void add ( relationship );
  int next_id( );

private:
  std::vector<relationship> relationships;
};
