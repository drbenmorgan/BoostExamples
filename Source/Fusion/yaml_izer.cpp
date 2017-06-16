//! Try yaml-ization without inheriting mixin

// Standard Library

// Third Party
// - yaml
#include "yaml-cpp/yaml.h"

// This Project
#include "YAMLizer.h"

// Base custom struct
struct ThreeVector {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};
BOOST_FUSION_ADAPT_STRUCT(ThreeVector, x, y, z)

// If we have a struct that's not adapted, will get
// compile time error, but can we make that helpful?
struct NoFusion {
  std::string nothing = "nobody";
};

// A class
class Time {
  public:
  Time(int h, int m, int s) : hour(h), minute(m), second(s) {}

  int get_hour() const {return hour;}
  void set_hour(int h) {hour = h;}
 
  int get_minute() const {return minute;}
  void set_minute(int m) {minute = m;}

  int get_second() const {return second;}
  void set_second(int s) {second = s;}

  void print() const {
    std::cout << "(" << hour <<":" << minute << ":" << second <<")\n";
  }

  private:
    int hour;
    int minute;
    int second;
};

// NB: ADT adaption works assuming that the class
// supplies get/set methods for all "properties"
// and that changing any of these will change/set
// any other private members needed to create the
// invariant. See, e.g.:
// http://stackoverflow.com/questions/26380420/boost-fusion-sequence-type-and-name-identification-for-structs-and-class
// Se also BOOST_FUSION_ADAPT_ASSOC_ADT, and Seewhat Boost.Hana
// can offer.
// Effectively the same as Python @property
// Can also implement via storing the persistable
// parts of a class in a struct, then streaming
// that. This may require the intrusive Mixin
// class.
//BOOST_FUSION_ADAPT_ADT(
 // Time,
 // (obj.get_hour(), obj.set_hour()),
//)


int main(int /*argc*/, const char */*argv*/[])
{
  ThreeVector foo;
  std::cout << YAMLizer::to_json(foo) <<std::endl;
  std::cout << YAMLizer::to_yaml(foo) << std::endl;

  // This will cause a compile-time error, but is the
  // message helpful? Yes, if functions are qualified
  // via enable_if, though messages could be clearer
  //std::cout << YAMLizer::to_yaml(bar) << std::endl;

  // Serialize/deserialize
  ThreeVector baz;
  baz.x = 1;
  baz.y = 2;
  baz.z = 3;

  std::string bazAsYaml = YAMLizer::to_yaml(baz);
  std::cout << "bazAsYaml: " << bazAsYaml << std::endl;

  ThreeVector bazRegen = YAMLizer::from_yaml<ThreeVector>(bazAsYaml);

  std::cout << "Regen baz: " << bazRegen.x << ", " << bazRegen.y << ", " << bazRegen.z << std::endl;

  return 0;
}

