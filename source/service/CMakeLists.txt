#cmake_minimum_required(VERSION 3.6)
#project(service)

#set(CMAKE_CXX_STANDARD 14)

include_directories(service)
#set(CXX_STANDARD 11)

#add_subdirectory(third_party/grpc)
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

set(gen_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/server.pb.cc")
set(gen_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/server.pb.h")
set(gen_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/server.grpc.pb.cc")
set(gen_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/server.grpc.pb.h")

set(gen_proto_python "${CMAKE_CURRENT_SOURCE_DIR}/service/server_pb2.py")
set(gen_grpc_python "${CMAKE_CURRENT_SOURCE_DIR}/service/server_pb2_grpc.py")

add_custom_command(
    OUTPUT ${gen_proto_srcs} ${gen_proto_hdrs} ${gen_grpc_srcs} ${gen_grpc_hdrs}
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS -I ${CMAKE_CURRENT_SOURCE_DIR}/service
        -I ${CMAKE_SOURCE_DIR}/third_party/grpc/third_party/protobuf/src/
        --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
        --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
        --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/service/server.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/service/server.proto
)

add_custom_command(
        OUTPUT ${gen_proto_python} ${gen_grpc_python}
        COMMAND python
        ARGS -m grpc_tools.protoc
             -I ${CMAKE_CURRENT_SOURCE_DIR}/service
             -I ${CMAKE_SOURCE_DIR}/third_party/grpc/third_party/protobuf/src/
             --python_out=${CMAKE_CURRENT_SOURCE_DIR}/service
             --grpc_python_out=${CMAKE_CURRENT_SOURCE_DIR}/service
             ${CMAKE_CURRENT_SOURCE_DIR}/service/server.proto
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/service/server.proto
)

add_library(service
        ${CMAKE_CURRENT_SOURCE_DIR}/service/server.proto
        ${CMAKE_CURRENT_SOURCE_DIR}/service/server_impl.cpp
        ${gen_proto_srcs}
        ${gen_proto_hdrs}
        ${gen_grpc_srcs}
        ${gen_grpc_hdrs}
#        ${gen_proto_python} Use manually
#        ${gen_grpc_python}
)
target_include_directories(service PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(service PUBLIC ./)
    #target_link_libraries(service grpc++ protobuf)
target_link_libraries(service grpc++ runtime)
