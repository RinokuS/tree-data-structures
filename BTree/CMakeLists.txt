cmake_minimum_required(VERSION 3.17)
project(BTree)

set(CMAKE_CXX_STANDARD 17)

add_executable(BTree main.cpp b_tree_node.h b_tree_node.cpp bin_serialization.h b_tree.h b_tree.cpp)