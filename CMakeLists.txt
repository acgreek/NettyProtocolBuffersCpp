# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (NettyProtoBuf)

IF(CMAKE_COMPILER_IS_GNUCC) 
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall" )
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX) 
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall" )
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

add_executable(client client.cc test.pb.cc)
add_executable(server server.cc test.pb.cc)
add_executable(test_prog test.cc test.pb.cc)
add_executable(test_gcov test.cc test.pb.cc)
set_target_properties(client PROPERTIES COMPILE_FLAGS "-ggdb3")
set_target_properties(test_prog test_gcov PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST")
set_target_properties(test_prog PROPERTIES COMPILE_FLAGS "-fPIC -pie -rdynamic -ggdb3 -std=c++0x ")
set_target_properties(test_prog PROPERTIES LINK_FLAGS "-fPIC -pie -rdynamic")

set_target_properties(test_gcov PROPERTIES COMPILE_FLAGS "-fPIC -pie -rdynamic -fprofile-arcs -ftest-coverage -ggdb3")
set_target_properties(test_gcov PROPERTIES LINK_FLAGS "-fPIC -pie -rdynamic -fprofile-arcs -ftest-coverage")

add_custom_target(coverage_clear_stats  DEPENDS test_gcov COMMAND lcov --directory CMakeFiles/test_gcov.dir --zerocounters)
add_custom_target(coverage_run_test DEPENDS coverage_clear_stats COMMAND ./test_gcov)
add_custom_target(coverage_genlcov DEPENDS coverage_run_test COMMAND lcov --directory CMakeFiles/test_gcov.dir --capture --output-file CMakeFiles/test_gcov.dir/app.info)
add_custom_target(coverage DEPENDS coverage_genlcov COMMAND genhtml CMakeFiles/test_gcov.dir/app.info -o html)

add_custom_target(test DEPENDS test_prog COMMAND ./test_prog)

add_executable(client_sync client_sync.cc test.pb.cc)
add_executable(server_sync server_sync.cc test.pb.cc)

SET(COMMON_SYSTEM_LIBS boost_system ssl pthread protobuf)
SET(TEST_LIBS ExtremeCUnit dl)

target_link_libraries (client ${COMMON_SYSTEM_LIBS} boost_chrono-mt)
target_link_libraries (server ${COMMON_SYSTEM_LIBS})
target_link_libraries (test_prog ${COMMON_SYSTEM_LIBS} ${TEST_LIBS})
target_link_libraries (test_gcov ${COMMON_SYSTEM_LIBS} ${TEST_LIBS})

target_link_libraries (client_sync ${COMMON_SYSTEM_LIBS})
target_link_libraries (server_sync ${COMMON_SYSTEM_LIBS} boost_thread-mt)



