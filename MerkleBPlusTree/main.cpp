#include <iostream>
#include "lib/MerkleBPlusTree.h"

struct bplus_tree_config {
    int order;
    int entries;
};

static void bplus_tree_get_put_test(bplus_tree<int, int> *tree)
{
    int i;

    fprintf(stderr, "\n> B+tree getter and setter testing...\n");

    tree->insert(24, 24);
    tree->insert(72, 72);
    tree->insert(1, 1);
    tree->insert(39, 39);
    tree->insert(53, 53);
    tree->insert(63, 63);
    tree->insert(90, 90);
    tree->insert(88, 88);
    tree->insert(15, 15);
    tree->insert(10, 10);
    tree->insert(44, 44);
    tree->insert(68, 68);
    tree->insert(74, 74);
    bplus_tree_dump(tree);

    tree->insert(10, 10);
    tree->insert(15, 15);
    tree->insert(18, 18);
    tree->insert(22, 22);
    tree->insert(27, 27);
    tree->insert(34, 34);
    tree->insert(40, 40);
    tree->insert(44, 44);
    tree->insert(47, 47);
    tree->insert(54, 54);
    tree->insert(67, 67);
    tree->insert(72, 72);
    tree->insert(74, 74);
    tree->insert(78, 78);
    tree->insert(81, 81);
    tree->insert(84, 84);
    bplus_tree_dump(tree);

    fprintf(stderr, "key:24 data:%d\n", tree->search(24));
    fprintf(stderr, "key:72 data:%d\n", tree->search(72));
    fprintf(stderr, "key:1 data:%d\n", tree->search(1));
    fprintf(stderr, "key:39 data:%d\n", tree->search(39));
    fprintf(stderr, "key:53 data:%d\n", tree->search(53));
    fprintf(stderr, "key:63 data:%d\n", tree->search(63));
    fprintf(stderr, "key:90 data:%d\n", tree->search(90));
    fprintf(stderr, "key:88 data:%d\n", tree->search(88));
    fprintf(stderr, "key:15 data:%d\n", tree->search(15));
    fprintf(stderr, "key:10 data:%d\n", tree->search(10));
    fprintf(stderr, "key:44 data:%d\n", tree->search(44));
    fprintf(stderr, "key:68 data:%d\n", tree->search(68));

    /* Not found */
    fprintf(stderr, "key:100 data:%d\n", tree->search(100));

    /* Clear all */
    fprintf(stderr, "\n> Clear all...\n");
    for (i = 1; i <= 100; i++) {
        tree->remove(i);
    }
    bplus_tree_dump(tree);

    /* Not found */
    fprintf(stderr, "key:100 data:%d\n", tree->search(100));
}

static void bplus_tree_insert_delete_test(bplus_tree<int, int> *tree)
{
    int i, max_key = 100;

    fprintf(stderr, "\n> B+tree insertion and deletion testing...\n");

    /* Ordered insertion and deletion */
    fprintf(stderr, "\n-- Insert 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        tree->insert(i, i);
    }
    bplus_tree_dump(tree);

    fprintf(stderr, "\n-- Delete 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        if (i == 100)
            std::cout << "";
        tree->remove(i);
    }
    bplus_tree_dump(tree);

    /* Ordered insertion and reversed deletion */
    fprintf(stderr, "\n-- Insert 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        tree->insert(i, i);
    }
    bplus_tree_dump(tree);

    fprintf(stderr, "\n-- Delete %d to 1, dump:\n", max_key);
    while (--i > 0) {
        tree->remove(i);
    }
    bplus_tree_dump(tree);

    /* Reversed insertion and ordered deletion */
    fprintf(stderr, "\n-- Insert %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->insert(i, i);
    }
    bplus_tree_dump(tree);

    fprintf(stderr, "\n-- Delete 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        tree->remove(i);
    }
    bplus_tree_dump(tree);

    /* Reversed insertion and reversed deletion */
    fprintf(stderr, "\n-- Insert %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->insert(i, i);
    }
    bplus_tree_dump(tree);

    fprintf(stderr, "\n-- Delete %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->remove(i);
    }
    bplus_tree_dump(tree);
}

static void bplus_tree_hash_test() {
    int i, max_key = 100;

    bplus_tree<int, int> *tree_a;
    bplus_tree<int, int> *tree_b;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree hash test.\n");

    /* Init b+tree */
    tree_a = bplus_tree_init<int, int>(config.order, config.entries);
    tree_b = bplus_tree_init<int, int>(config.order, config.entries);
    if (!tree_a || !tree_b) {
        fprintf(stderr, "Init failure!\n");
        exit(-1);
    }

    for (i = 1; i <= max_key; i++) {
        tree_a->insert(i, i);
        tree_b->insert(i, i);

        assert(tree_a->root->hash == tree_b->root->hash);
    }

    for (i = 1; i < max_key; i++) {
        tree_a->remove(i);
        tree_b->remove(i);

        assert(tree_a->root->hash == tree_b->root->hash);
    }
    assert(tree_a->root->hash == tree_b->root->hash);

    for (i = 1; i <= max_key; i++) {
        tree_a->insert(i, i);
    }
    for (i = max_key; i > 0; i--) {
        if (i == 11)
            std::cout << "";
        tree_b->insert(i, i);
    }
    assert(tree_a->root->hash != tree_b->root->hash);
}

static void bplus_tree_normal_test(void)
{
    bplus_tree<int, int> *tree;
    bplus_tree_config config;

    fprintf(stderr, "\n>>> B+tree normal test.\n");

    /* Init b+tree */
    config.order = 7;
    config.entries = 10;
    tree = bplus_tree_init<int, int>(config.order, config.entries);
    if (!tree) {
        fprintf(stderr, "Init failure!\n");
        exit(-1);
    }

    /* getter and setter test */
    bplus_tree_get_put_test(tree);

    /* insertion and deletion test */
    bplus_tree_insert_delete_test(tree);

    /* Deinit b+tree */
    bplus_tree_deinit(tree);
}

int main() {
    bplus_tree_normal_test();
    bplus_tree_hash_test();
    //bplus_tree_abnormal_test();
    return 0;
}