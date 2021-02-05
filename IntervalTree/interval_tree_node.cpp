#include <vector>
#include <algorithm>

#include "interval_tree_node.h"

using namespace std;

void heapify(vector<pair<int, int>> &vec, int n, int i, bool sort_by_first) {
    int ind_of_max = i,
            left_node = 2 * i + 1,
            right_node = 2 * i + 2;

    if (sort_by_first) {
        if (left_node < n && vec[left_node].first > vec[ind_of_max].first)
            ind_of_max = left_node;
        if (right_node < n && vec[right_node].first > vec[ind_of_max].first)
            ind_of_max = right_node;
    } else {
        if (left_node < n && vec[left_node].second > vec[ind_of_max].second)
            ind_of_max = left_node;
        if (right_node < n && vec[right_node].second > vec[ind_of_max].second)
            ind_of_max = right_node;
    }

    if (ind_of_max != i) {
        swap(vec[i], vec[ind_of_max]);
        heapify(vec, n, ind_of_max, sort_by_first);
    }
}

void heap_sort(vector<pair<int, int>> &vec, bool sort_by_first) {
    // node of the element will be 2*ind + 1 or 2*ind + 2
    // so we start our iteration from last node with leaves
    for (int i = vec.size() / 2 - 1; i >= 0; i--)
        heapify(vec, vec.size(), i, sort_by_first);

    for (int i = vec.size() - 1; i > 0; i--) {
        // Move current root (first element) to the end of unsorted part
        swap(vec[0], vec[i]);
        // re-heapify unsorted part
        heapify(vec, i, 0, sort_by_first);
    }
}

interval_tree_node::interval_tree_node(const vector<pair<int, int>> &intervals, int x_m) {
    x_median = x_m;
    vector<pair<int, int>> left_node_int, right_node_int;
    pair<int,int> left_node_mm({MAX_X,MIN_X}), right_node_mm({MAX_X,MIN_X});

    for (const auto& i: intervals) {
        if (i.second < x_median) {
            left_node_int.push_back(i);
            if (i.first < left_node_mm.first)
                left_node_mm.first = i.first;
            if (i.second > left_node_mm.second)
                left_node_mm.second = i.second;
        } else if (i.first > x_median) {
            right_node_int.push_back(i);
            if (i.first < right_node_mm.first)
                right_node_mm.first = i.first;
            if (i.second > right_node_mm.second)
                right_node_mm.second = i.second;
        } else if (i.first <= x_median && i.second >= x_median) {
            left_intervals.push_back(i);
            right_intervals.push_back(i);
        }
    }
    heap_sort(left_intervals, true);
    heap_sort(right_intervals, false);

    left_node = left_node_int.empty() ? nullptr :
            new interval_tree_node(left_node_int,(left_node_mm.first + left_node_mm.second) / 2);
    right_node = right_node_int.empty() ? nullptr :
            new interval_tree_node(right_node_int, (right_node_mm.first + right_node_mm.second) / 2);
}

int interval_tree_node::get_numb_of_intervals(int point) {
    int result = 0;
    if (point < x_median) {
        if (left_node)
            result = left_node->get_numb_of_intervals(point);
    } else if (point > x_median) {
        if (right_node)
            result = right_node->get_numb_of_intervals(point);
    }

    if (point <= x_median) {
        for (int i = left_intervals.size() - 1; i >= 0 ; --i) {
            if (left_intervals[i].first <= point)
                result++;
            else
                break;
        }
    } else if (point >= x_median) {
        for (auto &right_interval : right_intervals) {
            if (right_interval.second >= point)
                result++;
            else
                break;
        }
    }

    return result;
}