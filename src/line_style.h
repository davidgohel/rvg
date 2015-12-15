class line_style
{
public:
  line_style ( double, int, int, int );
  std::string a_tag();

private:
  double width;
  int col;
  int type;
  int join;
};
