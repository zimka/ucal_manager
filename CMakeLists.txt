cmake_minimum_required(VERSION 3.9)
project(ucal_manager)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(source)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC common)

add_subdirectory(tests)
add_test(NAME run_tests COMMAND run_tests)