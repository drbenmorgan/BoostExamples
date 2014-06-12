# - CMake script for BoostExamples Variant tests
#
# - Relocate need unit_tes
find_package(Boost REQUIRED unit_test_framework)

add_executable(basic_variant basic_variant.cpp)
add_executable(basic_visitor basic_visitor.cpp)
add_executable(property_variant property_variant.cpp)

add_executable(value_variant value_variant.cpp)
target_link_libraries(value_variant ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

