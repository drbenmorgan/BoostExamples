# - CMake script for BoostExamples ProgramOptions tests
#

# - Relocate program_options lib
find_package(Boost 1.58.0 QUIET REQUIRED program_options regex)

# - Examples
foreach(_p basic_positional multiple_help wildcards)
  add_executable(${_p} ${_p}.cpp)
  target_link_libraries(${_p} Boost::program_options)
endforeach()

target_link_libraries(wildcards Boost::regex)

