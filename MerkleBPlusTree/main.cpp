#include <iostream>
#include <chrono>
#include <fstream>
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
    tree->print();

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
    tree->print();

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
    tree->print();

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
    tree->print();

    fprintf(stderr, "\n-- Delete 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        if (i == 100)
            std::cout << "";
        tree->remove(i);
    }
    tree->print();

    /* Ordered insertion and reversed deletion */
    fprintf(stderr, "\n-- Insert 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        tree->insert(i, i);
    }
    tree->print();

    fprintf(stderr, "\n-- Delete %d to 1, dump:\n", max_key);
    while (--i > 0) {
        tree->remove(i);
    }
    tree->print();

    /* Reversed insertion and ordered deletion */
    fprintf(stderr, "\n-- Insert %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->insert(i, i);
    }
    tree->print();

    fprintf(stderr, "\n-- Delete 1 to %d, dump:\n", max_key);
    for (i = 1; i <= max_key; i++) {
        tree->remove(i);
    }
    tree->print();

    /* Reversed insertion and reversed deletion */
    fprintf(stderr, "\n-- Insert %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->insert(i, i);
    }
    tree->print();

    fprintf(stderr, "\n-- Delete %d to 1, dump:\n", max_key);
    for (i = max_key; i > 0; i--) {
        tree->remove(i);
    }
    tree->print();
}

static void bplus_tree_hash_test() {
    int i, max_key = 100;

    bplus_tree<int, int> *tree_a;
    bplus_tree<int, int> *tree_b;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree hash test.\n");

    /* Init b+trees */
    tree_a = bplus_tree<int, int>::init_tree(config.order, config.entries);
    tree_b = bplus_tree<int, int>::init_tree(config.order, config.entries);

    for (i = 1; i <= max_key; i++) {
        tree_a->insert(i, i);
        tree_b->insert(i, i);

        assert(strcmp(tree_a->root->hash, tree_b->root->hash) == 0);
    }

    for (i = 1; i < max_key; i++) {
        tree_a->remove(i);
        tree_b->remove(i);

        assert(strcmp(tree_a->root->hash, tree_b->root->hash) == 0);
    }
    assert(strcmp(tree_a->root->hash, tree_b->root->hash) == 0);

    for (i = 1; i <= max_key; i++) {
        tree_a->insert(i, i);
    }
    for (i = max_key; i > 0; i--) {
        tree_b->insert(i, i);
    }
    assert(strcmp(tree_a->root->hash, tree_b->root->hash) != 0);

    delete tree_a;
    delete tree_b;
}

static void get_change_set_test() {
    int i, max_key = 100;

    bplus_tree<int, int> *tree_a;
    bplus_tree<int, int> *tree_b;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree change_set test.\n");

    /* Init b+trees */
    tree_a = bplus_tree<int, int>::init_tree(config.order, config.entries);
    tree_b = bplus_tree<int, int>::init_tree(config.order, config.entries);

    for (i = 1; i <= max_key; i++) {
        tree_a->insert(i, i);
        tree_b->insert(i, i);

        auto change_set = tree_a->get_change_set(tree_b);
        assert(change_set.empty());
    }

    tree_a->remove(100);
    auto change_set = tree_a->get_change_set(tree_b);
    assert(change_set.size() == 1);

    for (i = max_key; i > 0; i--) {
        tree_a->remove(i);
        tree_b->remove(i);

        change_set = tree_a->get_change_set(tree_b);
        assert(change_set.empty());
    }

    delete tree_a;
    delete tree_b;
}

static void bplus_tree_normal_test()
{
    bplus_tree<int, int> *tree;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree normal test.\n");

    /* Init b+tree */
    tree = bplus_tree<int, int>::init_tree(config.order, config.entries);

    /* getter and setter test */
    bplus_tree_get_put_test(tree);

    /* insertion and deletion test */
    bplus_tree_insert_delete_test(tree);

    /* Deinit b+tree */
    delete tree;
}

static void bplus_tree_time_test_add() {
    bplus_tree<int, int> *tree;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree add time test.\n");

    /* Init b+tree */
    tree = bplus_tree<int, int>::init_tree(config.order, config.entries);

    std::ofstream os {"time_add.csv"};

    os << "With update" << '\n';

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= 10'000'000; i++) {
        tree->insert(i, i);

        if (i == 1000 || i == 10'000 || i == 100'000 || i == 1'000'000 || i == 2'000'000 ||
            i == 4'000'000 || i == 6'000'000 || i == 8'000'000 || i == 10'000'000) {
            os << std::fixed << std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - start).count() * 1e-9 <<
                    std::setprecision(9) << '\n';
        }
    }
}

static void bplus_tree_time_test_check_hash() {
    bplus_tree<int, int> *tree;
    bplus_tree_config config{7, 10};

    fprintf(stderr, "\n>>> B+tree check hash time test.\n");

    /* Init b+tree */
    tree = bplus_tree<int, int>::init_tree(config.order, config.entries);

    std::ofstream os {"time_check.csv"};

    os << "With update" << '\n';

    for (int i = 1; i <= 10'000'000; i++) {
        tree->insert(i, i);

        if (i == 1000 || i == 10'000 || i == 100'000 || i == 1'000'000 || i == 2'000'000 ||
        i == 4'000'000 || i == 6'000'000 || i == 8'000'000 || i == 10'000'000) {
            auto start = std::chrono::high_resolution_clock::now();
            auto tree_hash = tree->root->hash;
            os << std::fixed << std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now() - start).count() * 1e-9 <<
                    std::setprecision(9) << '\n';
        }
    }
}

int main() {
    bplus_tree_normal_test();
    bplus_tree_hash_test();
    get_change_set_test();

    bplus_tree_time_test_add();
    bplus_tree_time_test_check_hash();

    return 0;
}