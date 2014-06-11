# - CMake script for Boost PropertyTree tests
#
# Common Tools library
add_library(PropertyTreeTools SHARED PropertyTreeTools.cpp)

# Basic usage
add_executable(basic_ptree basic_ptree.cpp)
target_link_libraries(basic_ptree PropertyTreeTools)

# Custom data type
add_executable(custom_ptree custom_ptree.cpp)
target_link_libraries(custom_ptree PropertyTreeTools)


