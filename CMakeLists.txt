# Copyright 2019 - 2020 Alexander Grund
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at http://boost.org/LICENSE_1_0.txt)

# Builds the libraries for Boost.Nowide
#
# Options:
# Boost_NOWIDE_INSTALL
# Boost_NOWIDE_WERROR
# BUILD_TESTING
#
# Created target: Boost::nowide
#
# When not using CMake to link against the shared library on windows define -DBOOST_NOWIDE_DYN_LINK

cmake_minimum_required(VERSION 3.9)
# Version number starts at 10 to avoid conflicts with Boost version
set(_version 12.0.0)
if(BOOST_SUPERPROJECT_SOURCE_DIR)
  set(_version ${BOOST_SUPERPROJECT_VERSION})
endif()
project(boost_nowide VERSION ${_version} LANGUAGES CXX)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  # Make sure all binarys (especially exe/dll) are in one directory for tests to work
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

include(BoostAddOptions)
include(BoostAddWarnings)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(CTest)
endif()

# Using glob here is ok as it is only for headers
file(GLOB_RECURSE headers include/*.hpp)
if(WIN32)
  add_library(boost_nowide src/cstdio.cpp src/cstdlib.cpp src/iostream.cpp ${headers})
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(boost_nowide PUBLIC BOOST_NOWIDE_DYN_LINK)
  endif()
  target_compile_definitions(boost_nowide PUBLIC BOOST_NOWIDE_NO_LIB)
  target_include_directories(boost_nowide
      PUBLIC 
          $<INSTALL_INTERFACE:include>    
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(boost_nowide PUBLIC cxx_std_20)
  
  if(BOOST_SUPERPROJECT_SOURCE_DIR)
    target_link_libraries(boost_nowide PUBLIC Boost::config)
  else()
    find_package(Boost 1.56 REQUIRED)
    target_link_libraries(boost_nowide PUBLIC Boost::boost)
  endif()
  set_target_properties(boost_nowide PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    VERSION ${PROJECT_VERSION}
    EXPORT_NAME nowide
  )
else()
  add_library(boost_nowide INTERFACE)
  target_compile_definitions(boost_nowide INTERFACE BOOST_NOWIDE_NO_LIB)
  target_include_directories(boost_nowide
      INTERFACE 
          $<INSTALL_INTERFACE:include>    
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
  target_compile_features(boost_nowide INTERFACE cxx_std_20)
  
  if(BOOST_SUPERPROJECT_SOURCE_DIR)
    target_link_libraries(boost_nowide INTERFACE Boost::config)
  else()
    find_package(Boost 1.56 REQUIRED)
    target_link_libraries(boost_nowide INTERFACE Boost::boost)
  endif()
  set_target_properties(boost_nowide PROPERTIES
    EXPORT_NAME nowide
  )
endif()

add_library(Boost::nowide ALIAS boost_nowide)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(Boost_NOWIDE_INSTALL)
  include(InstallTargets)
  install_targets(TARGETS boost_nowide NAMESPACE Boost CONFIG_FILE ${PROJECT_SOURCE_DIR}/Config.cmake.in)
endif()
