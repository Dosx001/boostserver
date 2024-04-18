#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/variant.hpp>

struct Person {
  std::string name;
  int id;
  short age;
  std::string email;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & name;
    ar & id;
    ar & age;
    ar & email;
  }
};

struct Company {
  std::string name;
  long id;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & name;
    ar & id;
  }
};

using Message = boost::variant<Person, Company>;
