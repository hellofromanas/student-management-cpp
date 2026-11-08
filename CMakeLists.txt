cmake_minimum_required(VERSION 3.10)
project(StudentManagement CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sm_core src/student_manager.cpp)
target_include_directories(sm_core PUBLIC src)

add_executable(student_manager src/main.cpp)
target_link_libraries(student_manager PRIVATE sm_core)

enable_testing()
add_executable(test_manager tests/test_manager.cpp)
target_link_libraries(test_manager PRIVATE sm_core)
add_test(NAME unit COMMAND test_manager)

if(MSVC)
  target_compile_options(sm_core PRIVATE /W4)
else()
  target_compile_options(sm_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
