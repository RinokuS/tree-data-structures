#include <iostream>
#include <vector>

#include "interval_tree_node.h"

using namespace std;



int main() {
    int n, k, point, min_x = 1'000'000'000, max_x = 0;
    cin >> n;
    vector<pair<int, int>> intervals(n);

    for (auto& i: intervals) {
        cin >> i.first >> i.second;
        if (i.first < min_x)
            min_x = i.first;
        if (i.second > max_x)
            max_x = i.second;
    }
    auto tree = new interval_tree_node(intervals, (min_x + max_x) / 2);

    cin >> k;
    for (int i = 0; i < k; ++i) {
        cin >> point;
        cout << tree->get_numb_of_intervals(point) << '\n';
    }

    return 0;
}
