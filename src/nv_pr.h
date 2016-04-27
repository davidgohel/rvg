class nv_pr
{
public:
  nv_pr (int, int, std::string );
  std::string p_tag();
  std::string wps_tag();
  std::string x_tag();

  int id;
  int editable;
  std::string label;
};
