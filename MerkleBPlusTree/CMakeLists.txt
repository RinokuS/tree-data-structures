cmake_minimum_required(VERSION 3.17)
project(MerkleBPlusTree)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
add_executable(MerkleBPlusTree main.cpp)
target_link_libraries(MerkleBPlusTree OpenSSL::Crypto)
