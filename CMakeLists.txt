# - Main CMake script for BoostExamples
#
#-----------------------------------------------------------------------
# Copyright 2012-2016 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012-2016 University of Warwick
#
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)
#-----------------------------------------------------------------------
# Enforce an out-of-source build.
# Should be the first action in the CMakeLists.txt
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "BoostExamples requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

#-----------------------------------------------------------------------
# Project setup
#
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(BoostExamples)

#-----------------------------------------------------------------------
# Find Boost >= 1.58.0
#
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.58.0 REQUIRED COMPONENTS filesystem)

#-----------------------------------------------------------------------
# Global settings and recurse into example tree
#
# For convenience, enforce standard rather than features as this is
# only a testing project
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wshadow -Werror")
endif()

# - Testing, plus catch interface target
enable_testing()
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${PROJECT_SOURCE_DIR}/catch)
add_library(catch-main STATIC catch/catch-main.cpp)
target_link_libraries(catch-main INTERFACE catch)

add_subdirectory(Source)

