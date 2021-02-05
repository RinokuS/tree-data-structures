#include <vector>

struct interval_tree_node {
    static constexpr int MAX_X = 1'000'000'000, MIN_X = 0;
    interval_tree_node *left_node, *right_node;
    int x_median;
    std::vector<std::pair<int, int>> left_intervals, right_intervals;

    interval_tree_node(const std::vector<std::pair<int, int>> &intervals, int x_m);

    int get_numb_of_intervals(int point);
};

