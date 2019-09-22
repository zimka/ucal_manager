cmake_minimum_required(VERSION 3.9)
project(ucal_manager)

set(CMAKE_CXX_STANDARD 14)

set(USE_REAL_DEVICE OFF ) # "Set this to true, if you wish to build using real device. WINDOWS ONLY"
message("Root CMakeLists.txt: USE_REAL_DEVICE is set to ${USE_REAL_DEVICE}")

if (UNIX AND USE_REAL_DEVICE)
    message(FATAL_ERROR "Cannot use real device on Linux. Sad but true.")
endif ()

add_subdirectory(source)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE common runtime)

add_subdirectory(tests)
enable_testing()
