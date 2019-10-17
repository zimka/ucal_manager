cmake_minimum_required(VERSION 3.9)
project(ucal_manager)

set(CMAKE_CXX_STANDARD 14)

# "Set this to true, if you wish to build using real device. WINDOWS ONLY"
# "Requires manual source/device rebuild"
set(USE_REAL_DEVICE true ) 
message("Root CMakeLists.txt: USE_REAL_DEVICE is set to ${USE_REAL_DEVICE}")

if (UNIX AND USE_REAL_DEVICE)
    message(FATAL_ERROR "Cannot use real device on Linux. Sad but true.")
endif ()

add_subdirectory(source)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC common storage device runtime)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
endif ()
if (USE_REAL_DEVICE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC REAL_DEVICE)
endif()

add_subdirectory(tests)
enable_testing()
