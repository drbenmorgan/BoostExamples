# - CMake script for BoostExamples Units tests
#
add_executable(performance performance.cpp)
target_link_libraries(performance Boost::boost)

add_executable(runtime_units runtime_units.cpp)
target_link_libraries(runtime_units Boost::boost)
