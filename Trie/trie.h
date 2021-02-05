#include <string>

#include "trie_node.h"

class trie {
private:
    trie_node *root;
public:
    /** Initialize your data structure here. */
    trie();

    virtual ~trie();

    /** Inserts a word into the trie. */
    void insert(const std::string &word);

    /** Returns if the word is in the trie. */
    bool search(const std::string &word);

    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool starts_with(const std::string &prefix);
};