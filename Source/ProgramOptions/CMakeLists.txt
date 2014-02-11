# - CMake script for BoostExamples ProgramOptions tests
#

# - Relocate program_options lib
find_package(Boost 1.48.0 REQUIRED program_options)

# - Examples
add_executable(basic_positional basic_positional.cpp)
target_link_libraries(basic_positional ${Boost_LIBRARIES})

