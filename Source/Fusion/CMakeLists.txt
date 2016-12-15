# - CMake script for BoostExamples Units tests
#
add_executable(fusion_struct fusion_struct.cpp)
target_link_libraries(fusion_struct Boost::boost)

