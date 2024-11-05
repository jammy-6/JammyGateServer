list(APPEND CMAKE_PREFIX_PATH "/root/.local/lib/cmake")
#set(Protobuf_ROOT "/root/.local/lib/cmake")
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
message(STATUS "Using protobuf ${Protobuf_INCLUDE_DIRS}")
#find_package(protobuf CONFIG REQUIRED)


find_package(gRPC CONFIG REQUIRED)
find_package(c-ares CONFIG)
# find_package(Threads)

#
# Protobuf/Grpc source files
#

set("${PROJECT_NAME}_proto_dir" "${CMAKE_CURRENT_SOURCE_DIR}") 
file(GLOB_RECURSE "${PROJECT_NAME}_proto" "${${PROJECT_NAME}_proto_dir}/*.proto")

#
# Add Library target with protobuf sources
#
add_library(${PROJECT_NAME}_proto ${${PROJECT_NAME}_proto})
target_link_libraries(
    ${PROJECT_NAME}_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(
    ${PROJECT_NAME}_proto PUBLIC
        ${PROTOBUF_INCLUDE_DIRS} 
        ${CMAKE_CURRENT_BINARY_DIR}
)

#
# Compile protobuf and grpc files in mointor proto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ${PROJECT_NAME}_proto LANGUAGE cpp)
protobuf_generate(TARGET ${PROJECT_NAME}_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
