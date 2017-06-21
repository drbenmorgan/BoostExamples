# - CMake script for BoostExamples Units tests
#
add_executable(fusion_struct fusion_struct.cpp)
target_link_libraries(fusion_struct Boost::boost)

find_path(YAML_INCLUDE_DIRS yaml-cpp/yaml.h)
find_library(YAML_LIBRARIES yaml-cpp)
include_directories(${YAML_INCLUDE_DIRS})

if(YAML_INCLUDE_DIRS)
add_executable(fusion_yaml fusion_yaml.cpp)
target_link_libraries(fusion_yaml ${YAML_LIBRARIES})

add_executable(yaml_converters yaml_converters.cpp)
target_link_libraries(yaml_converters ${YAML_LIBRARIES})

add_executable(yaml_izer yaml_izer.cpp)
target_link_libraries(yaml_izer ${YAML_LIBRARIES})
endif()

