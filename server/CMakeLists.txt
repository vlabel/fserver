cmake_minimum_required(VERSION 2.8)

project(fserver)

set (SRC_DIR ${PROJECT_SOURCE_DIR}/src)

enable_testing()

set (SRC
 ${SRC_DIR}/server.cpp
 ${SRC_DIR}/io_service_pool.cpp
 ${SRC_DIR}/request_parser.cpp
 ${SRC_DIR}/reply.cpp
 ${SRC_DIR}/connection.cpp
 ${SRC_DIR}/storage_handler.cpp
 ${SRC_DIR}/constants.cpp
 ${SRC_DIR}/main.cpp)


add_executable(fserver ${SRC} )
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
target_link_libraries(fserver boost_system boost_thread pthread boost_program_options boost_filesystem)

add_test(test1 fserver --host ddlocalhost --port 9999 --net_threads 4 --io_threads 2 --directory ./)
add_test(test2 fserver --host ddlocalhost --port 9999 --net_threads 4 --io_threads 2 --directory ./no_dir)
add_test(test3 fserver --host ddlocalhost --port 23 --net_threads 4 --io_threads 2 --directory ./)
add_test(test4 fserver --host ddlocalhost --port 999 --net_threads -1 --io_threads 2 --directory ./)
add_test(test5 fserver --host ddlocalhost --port 23 --net_threads 4 --io_threads 0 --directory ./)

