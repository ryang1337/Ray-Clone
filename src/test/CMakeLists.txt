cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 20)

project(rayclone_test)

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

function(create_test file_name)
  add_executable(${file_name} ${file_name}.cc)
  include_directories(${file_name} PUBLIC "/Users/churd/programs/ray-clone")
  target_link_libraries(${file_name} GTest::gtest_main)
endfunction()

set(file_names register_function serializer)

foreach(file_name IN LISTS file_names)
  create_test(test_${file_name})
  gtest_discover_tests(test_${file_name})
endforeach()

# add_executable(test_register_function test_register_function.cc)
# include_directories(test_register_function PUBLIC "/Users/churd/programs/ray-clone")
# target_link_libraries(test_register_function GTest::gtest_main)
#
# add_executable(test_serializer test_serializer.cc)
# include_directories(test_serializer PUBLIC "/Users/churd/programs/ray-clone")
# target_link_libraries(test_serializer GTest::gtest_main)

# gtest_discover_tests(test_register_function)
