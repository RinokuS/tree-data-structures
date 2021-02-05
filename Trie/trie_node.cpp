#include "trie_node.h"

trie_node::trie_node() {
    is_word = false;
}

trie_node::~trie_node() {
    for (auto &el: children)
        delete el.second;
}