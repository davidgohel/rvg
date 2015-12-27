class a_color
{
public:
  a_color (int);
  int is_black();
  int is_visible();
  int has_alpha();
  std::string solid_fill();
  std::string solid_fill_w14();
  std::string w_color();
  int is_transparent();

private:
  int col;
  int alpha;
};
