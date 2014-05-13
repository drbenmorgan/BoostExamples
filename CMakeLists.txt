# - Main CMake script for BoostExamples
#
#-----------------------------------------------------------------------
# Copyright 2012 Ben Morgan <bmorgan.warwick@gmail.com>
# Copyright 2012 University of Warwick
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
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(BoostExamples)

#-----------------------------------------------------------------------
# Find Boost >= 1.48.0
#
find_package(Boost 1.48.0 REQUIRED)

#-----------------------------------------------------------------------
# Global settings and recurse into example tree
#
include_directories(${Boost_INCLUDE_DIRS})
enable_testing()

add_subdirectory(Source)

