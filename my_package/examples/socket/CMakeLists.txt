message(STATUS "Build socket examples ${CMAKE_BUILD_TYPE}")

# set option
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fpic -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -pthread")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(_SOCKET_SOURCE_DIR ${CMAKE_SOURCE_DIR}/network_lib/socket_lib)


# UDP Sender example
set(_UDP_SENDER_EXAMPLE udp_sender_test)
add_executable(${_UDP_SENDER_EXAMPLE} 
    udp_sender_example.cpp
 "udp_sender_example.cpp")

target_link_libraries(${_UDP_SENDER_EXAMPLE} 
PUBLIC 
    morai_socket
)

target_include_directories(${_UDP_SENDER_EXAMPLE}
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/socket
)

# UDP Receiver exampel
set(_UDP_RECEIVER_EXAMPLE udp_receiver_test)
add_executable(${_UDP_RECEIVER_EXAMPLE} 
    udp_receiver_example.cpp
 "udp_receiver_example.cpp")

target_link_libraries(${_UDP_RECEIVER_EXAMPLE} 
PUBLIC 
    morai_socket
)

target_include_directories(${_UDP_RECEIVER_EXAMPLE}
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/socket
)

# TCP Server example for v2
set(_TCP_SERVER_EXAMPLE tcp_server_test)
add_executable(${_TCP_SERVER_EXAMPLE} 
    tcp_server_example.cpp
 "tcp_server_example.cpp")

target_link_libraries(${_TCP_SERVER_EXAMPLE} 
PUBLIC 
    morai_socket
)

target_include_directories(${_TCP_SERVER_EXAMPLE}
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/socket
)

# # TCP Client example for v2
set(_TCP_CLIENT_EXAMPLE tcp_client_test)
add_executable(${_TCP_CLIENT_EXAMPLE} 
    tcp_client_example.cpp
 "tcp_client_example.cpp")

target_link_libraries(${_TCP_CLIENT_EXAMPLE} 
PUBLIC 
    morai_socket
)

target_include_directories(${_TCP_CLIENT_EXAMPLE}
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/socket
)