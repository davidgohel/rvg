class relationship
{
public:
  relationship ( int, std::string, std::string );
  std::string xml();
  int get_id();

private:
  int id;
  std::string type;
  std::string target;
};
