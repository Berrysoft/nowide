# Project testing to consume Boost.Nowide
cmake_minimum_required(VERSION 3.8.0)
project(nowide_example_project)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

find_package(nowide 1.0 REQUIRED)

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. test)

