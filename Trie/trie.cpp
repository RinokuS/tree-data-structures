#include "trie.h"

using namespace std;

trie::trie() {
    root = new trie_node();
}

trie::~trie() {
    delete root;
}

void trie::insert(const string &word) {
    trie_node* iter = root;

    for (const char& c: word) {
        if (iter->children.count(c) == 0)
            iter->children[c] = new trie_node();
        iter = iter->children[c];
    }

    iter->is_word = true;
}

bool trie::search(const string &word) {
    trie_node* iter = root;

    for (const char& c: word) {
        if (iter->children.count(c) == 0)
            return false;
        iter = iter->children[c];
    }

    return iter->is_word;
}

bool trie::starts_with(const string &prefix) {
    trie_node* iter = root;

    for (const char& c: prefix) {
        if (iter->children.count(c) == 0)
            return false;
        iter = iter->children[c];
    }

    return true;
}