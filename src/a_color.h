class a_color
{
public:
  a_color (int);
  int is_visible();
  std::string solid_fill();
  int is_transparent();

private:
  int col;
  int alpha;
};
