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

if (WIN32)
    set(OPENSSL_NO_ASM ON) # Maybe you don't need this
    add_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00) # And this, try commenting
else()
    set(gRPC_SSL_PROVIDER "package" CACHE STRING "Provider of ssl library")
endif(WIN32)
add_subdirectory(third_party/grpc)

add_subdirectory(source)
add_executable(${PROJECT_NAME} main.cpp)
if (UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC common storage runtime service)
if (USE_REAL_DEVICE)
    target_link_libraries(${PROJECT_NAME} PUBLIC device_real)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC device_mock)
endif()

add_subdirectory(tests)
enable_testing()
