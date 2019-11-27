# Builds the libraries for Boost.Nowide
# Automatically detects if build as standalone or as "part of boost" (with boost namespace)
#
# Options:
# NOWIDE_INSTALL
# NOWIDE_BUILD_TESTS
# NOWIDE_SYSTEM_INCLUDE
#
# Created targets:
# nowide::nowide ALIAS target to default build type
# nowide::static Static library if built, ALIAS on non-WIN32
# nowide::shared Shared library if built, ALIAS on non-WIN32
#
# When not using CMake to link against this on windows define
# -DBOOST_NOWIDE_DYN_LINK (non-standalone) or -DNOWIDE_EXPORT (standalone)

cmake_minimum_required(VERSION 3.9)
project(nowide VERSION 1.0.0 LANGUAGES CXX)

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(is_root_project ON)
  set(is_sub_project OFF)
else()
  set(is_root_project OFF)
  set(is_sub_project ON)
endif()

# Find out if we are beeing build as standalone or boost version
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/nowide)
  set(NOWIDE_STANDALONE ON)
else()
  set(NOWIDE_STANDALONE OFF)
endif()

option(NOWIDE_INSTALL "Install library" ${is_root_project})
option(NOWIDE_BUILD_TESTS "Build unit tests" ${is_root_project})
option(NOWIDE_SYSTEM_INCLUDE "Treat Boost.Nowide includes as system includes" ${is_sub_project})
if(NOT NOWIDE_STANDALONE)
  option(NOWIDE_USE_FILESYSTEM "Use Boost::filesystem for boost::filesystem::path support" ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(warningFlags -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(warningFlags -Wall -Werror)
elseif(MSVC)
  set(warningFlags /W3 /WX)
endif()

# Make sure all binarys (especially exe/dll) are in one directory for tests to work
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# On non-windows this is header only
# On windows we default to static as it is way easier to handle
# but the user can overwrite this by setting BUILD_SHARED=ON
set(argScope PUBLIC)
if(NOT WIN32)
  set(libName nowide)
  set(argScope INTERFACE)
  add_library(nowide INTERFACE)
  add_library(nowide::shared ALIAS nowide)
  add_library(nowide::static ALIAS nowide)
elseif(BUILD_SHARED)
  set(libName nowide-shared)
  add_library(nowide-shared SHARED)
  add_library(nowide::shared ALIAS nowide-shared)
  set_target_properties(nowide-shared PROPERTIES EXPORT_NAME shared)
  if(NOWIDE_STANDALONE)
    target_compile_definitions(nowide-shared PUBLIC NOWIDE_EXPORT)
  else()
    target_compile_definitions(nowide-shared PUBLIC BOOST_NOWIDE_DYN_LINK)
  endif()
else()
  set(libName nowide-static)
  add_library(nowide-static STATIC)
  add_library(nowide::static ALIAS nowide-static)
  set_target_properties(nowide-static PROPERTIES EXPORT_NAME static)
endif()
add_library(nowide::nowide ALIAS ${libName})

if(NOT NOWIDE_STANDALONE)
  # Default boost libs are static on windows and dynamic on linux
  if(WIN32 AND "${Boost_USE_STATIC_LIBS}" STREQUAL "")
    set(Boost_USE_STATIC_LIBS ON)
  endif()
  if(NOWIDE_USE_FILESYSTEM)
    # We don't really need Boost::filesystem as we only use its headers
    # but we need Boost::system until Boost 1.69 from where it is header only
    find_package(Boost 1.55 REQUIRED COMPONENTS system)
    target_link_libraries(${libName} ${argScope} Boost::boost Boost::system)
    target_compile_definitions(${libName} ${argScope} BOOST_NOWIDE_USE_FILESYSTEM BOOST_FILESYSTEM_NO_LIB)
  else()
    find_package(Boost 1.55 REQUIRED)
    target_link_libraries(${libName} ${argScope} Boost::boost)
  endif()
else()
  target_compile_features(${libName} ${argScope} cxx_std_11)
endif()

if(WIN32)
  # Using glob here is ok as it is only for headers
  file(GLOB_RECURSE NOWIDE_HEADERS include/*.hpp)
  target_sources(${libName} PRIVATE src/iostream.cpp ${NOWIDE_HEADERS})
  set_target_properties(${libName} PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_compile_options(${libName} PRIVATE ${warningFlags})
endif()

if(NOWIDE_SYSTEM_INCLUDE)
  set(nowideSystem SYSTEM)
else()
  set(nowideSystem "")
endif()
target_include_directories(${libName} ${nowideSystem} ${argScope}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(NOWIDE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

if(NOWIDE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  
  install(TARGETS ${libName}
    EXPORT nowideTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  set(configFile ${CMAKE_CURRENT_BINARY_DIR}/nowideConfig.cmake)
  set(versionFile ${CMAKE_CURRENT_BINARY_DIR}/nowideConfigVersion.cmake)
  set(configInstallDestination share/nowide)

  configure_package_config_file(
    Config.cmake.in
    ${configFile} 
    INSTALL_DESTINATION ${configInstallDestination}
  )
  write_basic_package_version_file(
    ${versionFile}
    COMPATIBILITY SameMajorVersion
  )

  install(FILES ${configFile} ${versionFile} DESTINATION ${configInstallDestination})

  install(
    EXPORT nowideTargets
    NAMESPACE "nowide::"
    DESTINATION ${configInstallDestination}
  )
endif()