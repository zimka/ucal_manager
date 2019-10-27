cmake_minimum_required(VERSION 3.9)
project(ucal_manager)

set(CMAKE_CXX_STANDARD 14)

# "Set this to true, if you wish to build using real device. WINDOWS ONLY"
# "Requires manual source/device rebuild"
set(USE_REAL_DEVICE FALSE )
message("Root CMakeLists.txt: USE_REAL_DEVICE is set to ${USE_REAL_DEVICE}")

if (UNIX AND USE_REAL_DEVICE)
    message(FATAL_ERROR "Cannot use real device on Linux. Sad but true.")
endif ()

add_subdirectory(source)
add_executable(${PROJECT_NAME} main.cpp)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
endif ()
if (USE_REAL_DEVICE)
    target_link_libraries(${PROJECT_NAME} PUBLIC common storage device_real runtime)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC common storage device_mock runtime)
endif()

add_subdirectory(tests)
enable_testing()
