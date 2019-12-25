cmake_minimum_required(VERSION 3.9)
project(ucal_manager)

set(CMAKE_CXX_STANDARD 14)

# "Set this to true, if you wish to build using real device. WINDOWS ONLY"
# "Requires manual source/device rebuild"
set(USE_REAL_DEVICE TRUE )
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
set(MAIN_SERVER ${PROJECT_NAME}_server)
set(MAIN_LOCAL ${PROJECT_NAME}_local)
set(CORE_LIB ${PROJECT_NAME}_lib)

add_subdirectory(source)
add_library(${CORE_LIB} INTERFACE)

if (UNIX)
    target_link_libraries(${CORE_LIB} INTERFACE pthread)

endif ()


if (USE_REAL_DEVICE)
    target_link_libraries(${CORE_LIB} INTERFACE device_real)
else()
    target_link_libraries(${CORE_LIB} INTERFACE device_mock)
endif()

target_link_libraries(${CORE_LIB} INTERFACE common storage runtime)

add_executable(${MAIN_SERVER} main_server.cpp)
target_link_libraries(${MAIN_SERVER} PUBLIC ${CORE_LIB} service)

add_executable(${MAIN_LOCAL} main_local.cpp)
target_link_libraries(${MAIN_LOCAL} PUBLIC ${CORE_LIB})

add_subdirectory(tests)
enable_testing()
