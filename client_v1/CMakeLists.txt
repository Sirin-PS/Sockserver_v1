cmake_minimum_required(VERSION 3.3)

project(rdb_client1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(
    ../include
)

add_executable(
  rdb_client1 client1.cpp)

