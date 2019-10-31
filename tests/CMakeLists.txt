
add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/Catch2 ${PROJECT_BINARY_DIR}/Catch2)
add_executable(run_tests run_tests.cpp test_common.cpp test_storage.cpp test_device.cpp test_runtime.cpp)
target_link_libraries(run_tests PUBLIC Catch2::Catch2 common storage device_mock runtime)
if (UNIX)
	target_link_libraries(run_tests PUBLIC pthread)
endif ()
target_include_directories(storage PUBLIC ${PROJECT_SOURCE_DIR}/third_party)
include(${PROJECT_SOURCE_DIR}/third_party/Catch2/contrib/Catch.cmake)

if (WIN32)
	add_executable(run_hardware_tests run_hardware_tests.cpp)
	target_link_libraries(run_hardware_tests PUBLIC Catch2::Catch2 common storage device_real)
endif()

