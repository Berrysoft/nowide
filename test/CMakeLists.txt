option(NOWIDE_RUN_WITH_WINE "Use wine to run tests" OFF)

set(NOWIDE_TESTS
  test_codecvt
  test_convert
  test_env
  test_fstream
  test_iostream
  test_stdio
)

function(nowide_add_test name target)
  if(NOWIDE_RUN_WITH_WINE)
    add_test(NAME ${name} COMMAND wine $<TARGET_FILE:${target}> ${ARGN})
  else()
    add_test(NAME ${name} COMMAND ${target} ${ARGN})
  endif()
endfunction()

foreach(TEST ${NOWIDE_TESTS})
  add_executable(${TEST} ${TEST}.cpp)
  target_link_libraries(${TEST} PRIVATE nowide::nowide)
  target_compile_options(${TEST} PRIVATE ${warningFlags})
  nowide_add_test(${TEST} ${TEST})
endforeach()

add_executable(test_env_win test_env.cpp)
target_link_libraries(test_env_win PRIVATE nowide::nowide)
target_compile_options(test_env_win PRIVATE ${warningFlags})
target_compile_definitions(test_env_win PRIVATE NOWIDE_TEST_INCLUDE_WINDOWS)
nowide_add_test(test_env_win test_env_win)

add_executable(test_system test_system.cpp)
target_link_libraries(test_system PRIVATE nowide::nowide)
target_compile_options(test_system PRIVATE ${warningFlags})
nowide_add_test(test_system_n test_system "-n")
nowide_add_test(test_system_w test_system "-w")

if(NOT NOWIDE_STANDALONE)
  find_package(Boost 1.55 COMPONENTS filesystem system)
  if(BOOST_FOUND)
    add_executable(test_fs test_fs.cpp)
    target_link_libraries(test_fs nowide::nowide Boost::filesystem)
    target_compile_options(test_fs PRIVATE ${warningFlags})
    nowide_add_test(test_fs test_fs)
  endif()
endif()
