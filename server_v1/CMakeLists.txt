cmake_minimum_required(VERSION 3.3)

project(rdb_server1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(
    ../include
)

add_executable(
  rdb_server1 mainServer.cpp serverCls.cpp serverCls.h)

target_link_libraries(
  rdb_server1 rocksdb)

