cmake_minimum_required(VERSION 2.8)

project(fserver)

set (SRC_DIR ${PROJECT_SOURCE_DIR}/src)


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
target_link_libraries(fserver boost_system boost_thread pthread boost_program_options)

