# Copyright 2019 - 2020 Alexander Grund
# Copyright 2020 Berrysoft
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt)

# Builds the libraries for Boost.Nowide
#
# Options:
# NOWIDE_INSTALL
# NOWIDE_WERROR
# BUILD_TESTING
#
# Created target: nowide::nowide
#
# When not using CMake to link against the shared library on windows define -DNOWIDE_DYN_LINK

cmake_minimum_required(VERSION 3.9)
# Version number starts at 10 to avoid conflicts with Boost version
set(_version 12.0.0)
project(nowide VERSION ${_version} LANGUAGES CXX)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  # Make sure all binarys (especially exe/dll) are in one directory for tests to work
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

include(NowideAddOptions)
include(NowideAddWarnings)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(CTest)
endif()

# Using glob here is ok as it is only for headers
file(GLOB_RECURSE headers include/*.hpp)
if(WIN32)
  add_library(nowide src/args.cpp src/cstdio.cpp src/cstdlib.cpp src/iostream.cpp ${headers})
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(nowide PUBLIC NOWIDE_DYN_LINK)
  endif()
  target_include_directories(nowide
      PUBLIC 
          $<INSTALL_INTERFACE:include>    
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(nowide PUBLIC cxx_std_17)
  
  set_target_properties(nowide PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    EXPORT_NAME nowide
  )
else()
  add_library(nowide INTERFACE)
  target_include_directories(nowide
      INTERFACE 
          $<INSTALL_INTERFACE:include>    
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(nowide INTERFACE cxx_std_17)
  
  set_target_properties(nowide PROPERTIES
    EXPORT_NAME nowide
  )
endif()

add_library(nowide::nowide ALIAS nowide)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(Boost_NOWIDE_INSTALL)
  include(InstallTargets)
  install_targets(TARGETS nowide NAMESPACE nowide CONFIG_FILE ${PROJECT_SOURCE_DIR}/Config.cmake.in)
endif()
