class line_style
{
public:
  line_style ( double, int, int, int, int );
  std::string a_tag();
  std::string svg_attr();

private:
  double width;
  int col;
  int type;
  int join;
  int end;
};
