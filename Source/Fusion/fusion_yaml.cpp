// Standard Library

// Third Party
// - yaml
#include "yaml-cpp/yaml.h"

// This Project
#include "YAMLMixin.h"

// structs
struct MyAlgorithm : public Mixin<MyAlgorithm> {
  int foo = 42;
  double bar = 3.14;
  std::string baz = "myalgo";
  // Use a node because there are cases where we can change the
  // implementation based on the "baz" string. We don't know
  // a priori what config that implementation takes, so we want to
  // delay the use/checking, e.g.
  // MyAlgorithm m;
  // m.from_yaml(...);
  // if (baz == "bob") {
  //   algoImpl = std::make_unique<bob>();
  //   algoImpl->configure(m.extra);
  // }
  YAML::Node extra;
};

BOOST_FUSION_ADAPT_STRUCT(MyAlgorithm, foo, bar, baz, extra);

struct AlgoImplementation : public Mixin<AlgoImplementation> {
  int foo = 100;
};

BOOST_FUSION_ADAPT_STRUCT(AlgoImplementation, foo);

// Can we parse/treat ini/properties as hierarchical, i.e. handle cases
// like
// [mysection]
// config = "foo" "bar" "baz"
// foo.key1 = 1
// foo.key2 = 2
// bar.key1...
//
// and treat this like
// mysection {
//  config [
//   {
//     name = "foo"
//     key1 = 1
//     key2 = 2
//  }
//  {
//    name = "bar"
//    key1 = 2
//    key2 = 4
//    ...
//  }
//  ]
//
// Reason is that data binding far easier for hierarchical structures
// YAML example does show how unhandled data can be dealt with though...
// i.e. could use postprocessing, albeit more work for user.
// Basically have to build mapping of key names to structs.
// Ultimately *always* end up with fixed key names.

// Try model from plugins
struct PluginConfig : public Mixin<PluginConfig> {
  std::string directory;
  bool autoload = true;
};

BOOST_FUSION_ADAPT_STRUCT(PluginConfig, directory, autoload);


struct LibraryLoaderConfig : public Mixin<LibraryLoaderConfig> {
  std::vector<std::string> plugins; // This will hold the keys we need to map
  std::map<std::string, PluginConfig> pluginConfig; // to this, but read separately!

  bool from_yaml(YAML::Node& node) {
    std::cout << " Calling LibraryLoaderConfig fom_yaml" << std::endl;
    bool core = Mixin<LibraryLoaderConfig>::from_yaml(node);
    if (core) std::cout << "got plugins sequence o.k." << std::endl;

    // Probably more direct way to do this using a specialization
    // or custom class/struct to parse this type of key-from-previous-value pattern.
    for (const auto& key: plugins) {
      std::cout << "Trying to parse key " << key << std::endl;
      PluginConfig c;
      YAML::Node sub = node[key];
      c.from_yaml(sub);
      pluginConfig[key] = c;
      std::cout << c.directory << " " << c.autoload << std::endl;
    }

    return core;
  }

  std::string to_yaml() {
    std::string core = Mixin<LibraryLoaderConfig>::to_yaml();

    YAML::Node input;
    for (auto& k : pluginConfig) {
      input[k.first] = k.second;
    }

    YAML::Emitter emit;
    emit << input;
    core += "\n";
    core += emit.c_str();
    return core;
  }

};

BOOST_FUSION_ADAPT_STRUCT(LibraryLoaderConfig, plugins);




int main(int /*argc*/, const char */*argv*/[])
{
  MyAlgorithm algo;
  std::cout << algo.to_yaml() << std::endl;
  std::cout << algo.to_json() << std::endl;

  std::string inYaml {"bar: 4.13\nfoo: 24\nbaz: 'blerg'\nextra:\n foo: 23"};
  algo.from_yaml(inYaml);
  std::cout << algo.to_yaml() << std::endl;

  AlgoImplementation s;
  s.from_yaml(algo.extra);
  std::cout << s.to_json() << std::endl;


  LibraryLoaderConfig userLibConfig;
  YAML::Node input;
  auto keys = std::vector<std::string> {"foo", "bar", "baz"};
  input["plugins"] = keys;

  for (auto& k : keys) {
    PluginConfig x;
    x.directory = "/" + k;
    input[k] = x;
  }

  userLibConfig.from_yaml(input);

  std::cout << userLibConfig.to_yaml() << std::endl;


  return 0;
}

