//! Try structs holding maps/arrays

// Standard Library
#include <array>

// Third Party
// - yaml
#include "yaml-cpp/yaml.h"

// This Project
#include "YAMLMixin.h"

// Base custom struct
struct ThreeVector : public Mixin<ThreeVector> {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};
BOOST_FUSION_ADAPT_STRUCT(ThreeVector, x, y, z)

// Now stick the custom struct into a map
struct Axes : public Mixin<Axes> {
  std::map<std::string, ThreeVector> base;
};
BOOST_FUSION_ADAPT_STRUCT(Axes, base)

struct AxisArray : public Mixin<AxisArray> {
  std::array<ThreeVector,4> axes;

  AxisArray() {
    ThreeVector x;
    x.x=1;
    x.y=0;
    x.z=0;
    ThreeVector y;
    y.x=0;
    y.y=1;
    y.z=0;
    ThreeVector z;
    z.x=0;
    z.y=0;
    z.z=1; 
    axes[0] = x;
    axes[1] = y;
    axes[2] = z;

  }

  void print() const {
    std::cout << "AxisArray {\n";
    for (auto elem: axes) std::cout << "  (" << elem.x << "," << elem.y << "," << elem.z << ")\n";
    std::cout << "}\n";
  }
};
BOOST_FUSION_ADAPT_STRUCT(AxisArray,axes)

// std::array isn't supported out-the-box by yaml-cpp,
// But we can supply a converter
namespace YAML {
  template <typename T, size_t N>
  struct convert<std::array<T,N>> {
    static Node encode(const std::array<T,N>& rhs) {
      Node node;
      for (auto elem: rhs) {
        node.push_back(elem);
      }
      return node;
    }

    static bool decode(const Node& node, std::array<T,N>& rhs) {
      if(!node.IsSequence() || node.size() != N) {
        return false;
      }
      for(size_t i=0; i<N; ++i) {
        rhs[i] = node[i].as<T>();
      }
      return true;
    }
  };
}

int main(int /*argc*/, const char */*argv*/[])
{
  ThreeVector foo;
  std::cout << foo.to_json() <<std::endl;
  std::cout << foo.to_yaml() << std::endl;

  Axes bar;
  bar.base["zenith"] = ThreeVector {};
  bar.base["pole"] = ThreeVector {};

  std::cout << "Yaml/Jsonize Axes:\n";
  std::cout << bar.to_yaml() << "\n";
  std::cout << bar.to_json() << "\n";

  AxisArray baz;
  std::cout << "Raw AxisArray:\n";
  baz.print();
  std::cout << "Yaml/Jsonize AxisArray:\n";
  std::cout << baz.to_yaml() << "\n";
  std::cout << baz.to_json() << "\n";

  AxisArray baz_regen;
  std::string as_yaml = baz.to_yaml();
  bool res = baz_regen.from_yaml(as_yaml);
  std::cout << "regen = " << std::boolalpha << res << "\n";
  baz_regen.print();


  return 0;
}

