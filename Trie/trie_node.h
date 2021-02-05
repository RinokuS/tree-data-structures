#include <unordered_map>

struct trie_node {
    std::unordered_map<char, trie_node*> children;
    bool is_word;

    trie_node();

    virtual ~trie_node();
};