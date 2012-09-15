# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (HELLO)

add_executable(client client.cc)
add_executable(server server.cc)


target_link_libraries (client boost_system ssl pthread protobuf)
target_link_libraries (server boost_system ssl pthread protobuf)

