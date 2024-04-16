#include <string>
#include <variant>

namespace boost {
namespace serialization {
template <class Archive, typename... Types>
void serialize(Archive &ar, std::variant<Types...> &variant,
               const unsigned int version) {
  int index = variant.index();
  ar & index;
  std::visit([&ar](auto &&value) { ar & value; }, variant);
}
} // namespace serialization
} // namespace boost

enum MessageType {
  PERSON,
  COMPANY,
};

struct Person {
  std::string name;
  short age;
  long id;
  std::string email;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & name;
    ar & age;
    ar & id;
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

struct Message {
  MessageType type;
  std::variant<Person, Company> payload;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar & type;
    ar & payload;
  }
};
