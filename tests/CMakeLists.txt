
set(USE_REAL_DEVICE OFF) # This overrides variable in the root CMakeLists.txt. Here it should be always OFF!
message("Test CMakeLists.txt: USE_REAL_DEVICE is set to ${USE_REAL_DEVICE}")
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/Catch2 ${PROJECT_BINARY_DIR}/Catch2)
add_executable(run_tests run_tests.cpp test_common.cpp test_storage.cpp test_device.cpp)
target_link_libraries(run_tests PUBLIC Catch2::Catch2 common storage device)
target_include_directories(storage PUBLIC ${PROJECT_SOURCE_DIR}/third_party)

include(${PROJECT_SOURCE_DIR}/third_party/Catch2/contrib/Catch.cmake)
include(CTest)
catch_discover_tests(run_tests)