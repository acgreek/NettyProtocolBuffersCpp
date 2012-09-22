# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (HELLO)

IF(CMAKE_COMPILER_IS_GNUCC) 
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall" )
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX) 
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall" )
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

add_executable(client client.cc)
add_executable(server server.cc)
add_executable(test test.cc test.pb.cc)
set_target_properties(test PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST")
set_target_properties(test PROPERTIES COMPILE_FLAGS "-fPIC -pie -rdynamic -ggdb3")
set_target_properties(test PROPERTIES LINK_FLAGS "-fPIC -pie -rdynamic")

add_executable(client_sync client_sync.cc test.pb.cc)
add_executable(server_sync server_sync.cc test.pb.cc)

target_link_libraries (client boost_system ssl pthread protobuf)
target_link_libraries (server boost_system ssl pthread protobuf)
target_link_libraries (test boost_system ssl pthread protobuf ExtremeCUnit dl)

target_link_libraries (client_sync boost_system ssl pthread protobuf)
target_link_libraries (server_sync boost_thread-mt boost_system ssl pthread protobuf)



