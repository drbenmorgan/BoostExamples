# - CMake script for BoostExamples ProgramOptions tests
#

# - Relocate program_options lib
find_package(Boost 1.48.0 REQUIRED program_options)

# - Examples
foreach(_p basic_positional multiple_help)
  add_executable(${_p} ${_p}.cpp)
  target_link_libraries(${_p} ${Boost_LIBRARIES})
endforeach()

