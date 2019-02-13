add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/Catch2 ${PROJECT_BINARY_DIR}/Catch2)
add_executable(run_tests run_tests.cpp test_common.cpp)
target_link_libraries(run_tests PUBLIC Catch2::Catch2 common)
