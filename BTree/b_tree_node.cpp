#include <fstream>
#include <string>
#include <vector>

#include "bin_serialization.h"
#include "b_tree_node.h"

using namespace std;

unsigned long NODES_COUNT = 1; // переменная для генирации названий нодов

b_tree_node::b_tree_node() {
    cnt_keys = 0;
    file_name = "Node_" + to_string(NODES_COUNT++);
    is_leaf = true;

    keys = vector<int>(2*t - 1);
    values = vector<int>(2*t - 1);
    children = vector<string>(2*t);
}

b_tree_node::b_tree_node(const string &fn) {
    file_name = fn;
    cnt_keys = 0;
    is_leaf = true;

    read();
}

void b_tree_node::split_child(b_tree_node& x, long i, b_tree_node& y) {
    b_tree_node z;
    z.is_leaf = y.is_leaf;
    z.cnt_keys = t - 1; // создаем новый нод
    z.parent = y.parent;

    copy_keys(z, y, 0, t, t - 1); // переносим в него половину элементов из y

    if (!y.is_leaf){
        for (size_t j = 0; j <= t - 1; ++j) {
            z.children[j] = y.children[j + t];
            b_tree_node temp(z.children[j]);
            temp.parent = z.file_name;
            temp.write();
        }
    }
    y.cnt_keys = t - 1;

    for (long j = long(x.cnt_keys); j > i; --j)
        x.children[j + 1] = x.children[j];
    x.children[i+1] = z.file_name;
    // разделитель засовываем в родителя
    for (long j = long(x.cnt_keys) - 1; j >= i; --j) {
        x.keys[j + 1] = x.keys[j];
        x.values[j + 1] = x.values[j];
    }

    x.keys[i] = y.keys[t-1];
    x.values[i] = y.values[t-1];

    x.cnt_keys++;

    y.write();
    z.write();  // сохраняем все ноды
    x.write();
}

void b_tree_node::read() {
    ifstream in{bin_files_path + '/' + file_name + ".bin", ios_base::binary};

    bin_serialization::deserialize(in, cnt_keys);
    bin_serialization::deserialize(in, is_leaf);
    bin_serialization::deserialize(in, parent);

    bin_serialization::deserialize(in, keys);
    bin_serialization::deserialize(in, values);
    bin_serialization::deserialize(in, children);
}

void b_tree_node::write() const {
    ofstream out{bin_files_path + '/' + file_name + ".bin", ios_base::binary};

    bin_serialization::serialize(cnt_keys, out);
    bin_serialization::serialize(is_leaf, out);
    bin_serialization::serialize(parent, out);

    bin_serialization::serialize(keys, out);
    bin_serialization::serialize(values, out);
    bin_serialization::serialize(children, out);
}

[[nodiscard]] string b_tree_node::get_name() const {
    return file_name;
}

void b_tree_node::copy_key(b_tree_node &dest, pair<int, int> obj,
                     size_t ind_dest) {
    dest.keys[ind_dest] = obj.first;
    dest.values[ind_dest] = obj.second;
}

void b_tree_node::copy_keys(b_tree_node &dest, b_tree_node &obj,
                      size_t ind_dest, size_t ind_obj, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        dest.keys[ind_dest + i] = obj.keys[ind_obj + i];
        dest.values[ind_dest + i] = obj.values[ind_obj + i];
    }
}