Properties and Data Binding
===========================

The core use case for Properties is to provide a human readable,
runtime configuration language for objects. For example

```cpp
Properties p;
p.read("config_for_foo.conf");

Foo f(p);

...

Foo g;
g.initialize(p);

```

Currently, authors of objects like Foo must process and validate the contents
of the supplied Properties instance by hand, which can lead to awkward
coding and much boilerplate. For example:

```cpp
struct FooConfig {
  int a = 42;
  double b = 3.14;
};

void Foo::initialize(const Properties& p) {
  FooConfig c;

  if (c.has_key("a") && c.is_integer("a")) {
    c.a = c.get_integer("a");
  } else {
    ???
  }

  ...
}
```


As the schema, i.e. structure, of the Properties
object is crucial to easy and correct usage, we'd like to make this easier
for both developers and users. The way to do this is through better and automated
"data binding", i.e. mapping a human readable representation of an object's
configuration to the actual in-memory object.

```cpp
struct FooConfig {
  int a = 42;
  double b = 3.14;
};

void Foo::initialize(const Properties& p) {
  FooConfig c = from_properties<FooConfig>(p);
  ...
}
```

Examples of such data binding abilities are known for configuration data
such as JSON, YAML and XML, but what about Properties? Significant
issues include:

- Properties default format is non-hierarchical, so nested data structures
  cannot be bound easily.
- Properties can store additional custom types such as filesystem paths and
  physical quantities (albeit as *interpretations* of strings and floating
  point numbers respectively).

The FermiLab "FHiCL" language could provide a substitute, but does
require an additional FNAL only library (cetlib), C++14 and may not
work on all required platforms. Could it be stripped down to provide
a C++/Boost only implementation?

Properties vs JSON vs YAML
==========================

The Properties/Multiproperties grammar shares many features with the
JSON and YAML markups, the major difference being that it is only
pseudo-hierarchical. In Properties, a "document" is 1-N key-specification-value
triplets:

```
a : int = 42
b : string = "hello"
c : real as length = 3.14 mm
d : int[4] = 1 2 3 4
e : string = "config"
e.f : int = 24
e.g : real = 4.13
```

The keys are only mapped as a flat structure, so in the above example the
`e` key and "subkeys" `e.f` and `e.g` do not form a tree in a C++ `Properties`
object. The C++ API for Properties only provides member functions for querying
keys and exporting those beginning with certain patterns. Here, the use of "." to
separate keys is arbitrary, and the caller must know this is used if they wish
to try and create a hireachy from the flat map.

In JSON, the above data could be represented as

```json
{
  "a" : 42,
  "b" : "hello",
  "c" : [3.14, "mm"],
  "d" : [1, 2, 3, 4],
  "e" : {
    "id" : "config",
    "f" : 24,
    "g" : 4.13
  }
}
```

The value-with-unit key `c` could also be represented as `"c" : {"value" : 3.14, "unit" : "mm"}`.

In YAML, the structure looks like

```yaml
---
a: 42
b: "hello"
c:
  value: 3.14
  unit: "mm"
d: [1, 2, 3, 4]
e:
  id: "config"
  f: 24
  g: 4.13
...
```

We may also use explicit global or local tags in YAML:

```yaml
---
a: !!int 42
b: !!str "hello"
c: !length
  value: 3.14
  unit: "mm"
d: !!seq [1, 2, 3, 4]
e: !eobject
  id: "config"
  f: 24
  g: 4.13
...
```

Local tags like `!length` mean that a converter must be available, and
this depends on the YAML parser/implementation in use. Overall, YAML with explicit tags is closest to Properties.

Multiproperties effectively only adds one level of hierarchy to
Properties, i.e. being composed of `Section`s each of which is
a Properties instance. Again, no hierachy other than this
sectioning is present or easily accessible through the API.

Can Properties Be Treated Hierarchically?
=========================================

Would require iteration over all keys, splitting them by ".".
If the split result has more than 1 entry, create a "node" and
push the subkeys into that. Repeat until there are only
leaves. NB: requires a hierarchical structure to be present!

Probably need to do this whatever, as any migration would require this
in order to convert from old to new style.

