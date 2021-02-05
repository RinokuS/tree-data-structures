cmake_minimum_required(VERSION 3.17)
project(Trie)

set(CMAKE_CXX_STANDARD 17)

add_executable(Trie main.cpp trie.h trie_node.h trie_node.cpp trie.cpp)