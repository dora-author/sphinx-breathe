cmake_minimum_required(VERSION 3.5.1)

message(STATUS "gRPC Server example  ${CMAKE_BUILD_TYPE}")

set(GRPC_SERVER_EXAMPLE "grpc_server_example")


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)

# set option
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

file(GLOB_RECURSE _lib_files ${CMAKE_SOURCE_DIR}/utils/*)
file(GLOB _local_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# message(STATUS "+++ lib : ${_lib_files}")
# message(STATUS "+++ local : ${_local_files}")

add_executable(${GRPC_SERVER_EXAMPLE}
    ${_lib_files}
    ${_local_files}
)

target_include_directories(${GRPC_SERVER_EXAMPLE}
PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/grpc
    ${MORAI_GRPC_LIB_ROOT_DIR}
) 

target_link_libraries(${GRPC_SERVER_EXAMPLE} 
PUBLIC 
    morai_grpc
)
