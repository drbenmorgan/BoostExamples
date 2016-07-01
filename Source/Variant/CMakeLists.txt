# - CMake script for BoostExamples Variant tests
#
# - Relocate need unit_tes
find_package(Boost QUIET REQUIRED unit_test_framework)

add_executable(basic_variant basic_variant.cpp)
target_link_libraries(basic_variant Boost::boost)

add_executable(basic_visitor basic_visitor.cpp)
target_link_libraries(basic_visitor Boost::boost)

add_executable(property_variant property_variant.cpp)
target_link_libraries(property_variant Boost::boost)

add_executable(value_variant value_variant.cpp)
target_link_libraries(value_variant Boost::unit_test_framework)

