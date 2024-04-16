#include <string>

struct Person {
  std::string name;
  short age;
  long id;
  std::string email;
  double hash;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & name;
    ar & age;
    ar & id;
    ar & email;
    ar & hash;
  }
};
