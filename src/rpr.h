class rpr
{
public:
  rpr (double, int, int, int, std::string );
  std::string a_tag();
  std::string w_tag();

  double size;
  int italic;
  int bold;
  int col;
  std::string fontname;
};
