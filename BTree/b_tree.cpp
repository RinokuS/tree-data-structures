#include <string>
#include <optional>
#include <algorithm>

#include "b_tree.h"

using namespace std;

optional<pair<b_tree_node, size_t>> b_tree::search_nodes(b_tree_node semi_root, int key) {
    size_t i = 0;
    while(i < semi_root.cnt_keys && key > semi_root.keys[i])
        i++;

    if (i < semi_root.cnt_keys && key == semi_root.keys[i])
        return {make_pair(move(semi_root), i)};

    if (semi_root.is_leaf)
        return {};
    else{
        auto semi_root2 = b_tree_node(semi_root.children[i]);
        semi_root2.parent = semi_root.file_name;

        return search_nodes(move(semi_root2), key);
    }
}

string b_tree::remove_in_good_leaf(b_tree_node &node, int key) {
    int j = 0;

    while (node.keys[j] != key)
        j++; // ищем индекс элемента

    string res = to_string(node.values[j]); // сохраняем значение элемента для вывода
    for (long i = j; i < ((long)node.cnt_keys) - 1; ++i) {
        node.keys[i] = node.keys[i + 1]; // сдвигаем диапазона на элемент влево, если удаляемый был не последним
        node.values[i] = node.values[i + 1];
    }

    node.cnt_keys--; // уменьшаем количество элементов
    node.write();

    return res;
}

string b_tree::remove_in_leaf(b_tree_node &node, int key) {
    string res;

    if (node.cnt_keys > t - 1 || root->file_name == node.file_name) {
        res = remove_in_good_leaf(node, key); // если лист "хороший" - вызываем соответсвующий метод
    } else {
        b_tree_node parent(node.parent);
        int i = 0;
        rebase(parent, node); // если лист пуст (у него t - 1 элемент) - вызываем ребейз дерева

        while (i < parent.cnt_keys && key > parent.keys[i])
            i++;

        if (node.cnt_keys == 0) { // махинации по замене ноды, ибо если произойдет ребейз с левым братом, то наша активная - удалится
            node = b_tree_node(parent.children[i]);
            int j = 0;

            for (j = 0; j < node.cnt_keys; ++j) {
                if (node.keys[j] == key)
                    break;
            }

            if (j == node.cnt_keys)
                node = b_tree_node(parent.children[i - 1]);
        }


        res = remove_in_good_leaf(node, key); // после ребейза нода уже непуста, значит вызываем удаление из хорошего листа
    }

    root->read();
    return res;
}

string b_tree::remove_in_good_nonleaf(b_tree_node &node, int key) {
    int j = 0;
    while (node.keys[j] != key)
        j++;
    // просто свапаем наш элемент с самым ближайшим к нему слева (он всегда будет в листке)
    b_tree_node left_node(node.children[j]);
    while(!left_node.is_leaf)
        left_node = b_tree_node(left_node.children[left_node.cnt_keys]);

    swap(node.keys[j], left_node.keys[left_node.cnt_keys - 1]);
    swap(node.values[j], left_node.values[left_node.cnt_keys - 1]);

    node.write();
    left_node.write();

    return remove_in_leaf(left_node, key); // удаляем наш элемент уже из листка, это мы умеем
}

string b_tree::remove_in_nonleaf(b_tree_node &node, int key) {
    string res;

    if (node.cnt_keys > t - 1) { // если нода непуста, либо является корнем - вызываем метод удаления из хорошей ноды
        res = remove_in_good_nonleaf(node, key);
    } else if (node.file_name == root->file_name) {
        res = remove_in_good_nonleaf(node, key);

        if (node.cnt_keys == 0){
            merge(node, 0);
            root->file_name = node.children[0];
        }
    } else {
        b_tree_node parent(node.parent);
        int i = 0;

        rebase(parent, node); // вызываем любимый ребейз, если в ноде все же t-1 элемент

        while (i < parent.cnt_keys && key > parent.keys[i])
            i++;

        if (node.cnt_keys == 0)
            node = b_tree_node(parent.children[i]);
        res = remove_in_good_nonleaf(node, key);
    }

    root->read();
    return res;
}

void b_tree::merge(b_tree_node &parent, int index_of_link) {
    b_tree_node left_node(parent.children[index_of_link]);
    b_tree_node right_node(parent.children[index_of_link + 1]);
    // если в родителе меньше t элементов, то делаем ребейз (ибо нам нужно достать соединяющий элемент)
    if (parent.file_name != root->file_name && parent.cnt_keys == t - 1) {
        b_tree_node temp_parent(parent.parent);
        int i = 0, key = parent.keys[0];

        rebase(temp_parent, parent);
        while (i < temp_parent.cnt_keys && key > temp_parent.keys[i])
            i++;

        if (parent.cnt_keys == 0)
            parent = b_tree_node(temp_parent.children[i]);

        for (int j = 0; j < parent.cnt_keys; ++j) {
            if (parent.children[j] == left_node.file_name){
                index_of_link = j;
                left_node = b_tree_node(parent.children[index_of_link]);
                right_node = b_tree_node(parent.children[index_of_link + 1]);
                break;
            }
        }
    }
    // копируем все элементы в левую ноду
    b_tree_node::copy_key(left_node,
                          make_pair(parent.keys[index_of_link],
                                    parent.values[index_of_link]), left_node.cnt_keys);
    b_tree_node::copy_keys(left_node, right_node,
                           left_node.cnt_keys + 1, 0, right_node.cnt_keys);

    if (!left_node.is_leaf) {
        for (int j = 0; j < right_node.cnt_keys + 1; ++j) {
            left_node.children[left_node.cnt_keys + 1 + j] = right_node.children[j];
            b_tree_node temp(right_node.children[j]);
            temp.parent = left_node.file_name;
            temp.write();
        }
    }

    left_node.cnt_keys += right_node.cnt_keys + 1;

    for (long j = index_of_link; j < parent.cnt_keys - 1; ++j) {
        parent.keys[j] = parent.keys[j + 1];
        parent.values[j] = parent.values[j + 1];
    }

    for (int j = index_of_link + 1; j < parent.cnt_keys; ++j)
        parent.children[j] = parent.children[j + 1];
    // делаем правую ноду мертвой - ставим значение ключей = 0
    parent.cnt_keys--;
    right_node.cnt_keys = 0;

    if (parent.cnt_keys == 0 && parent.file_name == root->file_name)
        root->file_name = left_node.file_name;

    left_node.write();
    right_node.write();
    parent.write();

    root->read();
}

void b_tree::rebase_with_right(b_tree_node &parent, b_tree_node &problem_child,
                              b_tree_node &right_bro, int index) {
    b_tree_node::copy_key(problem_child, // переносим ключ-разделитель в проблемный нод
                          make_pair(parent.keys[index],
                                    parent.values[index]), problem_child.cnt_keys);
    problem_child.cnt_keys++;
    b_tree_node::copy_key(parent, // переносим самый левый элемент правого брата на место разделителя
                          make_pair(right_bro.keys[0],
                                    right_bro.values[0]), index);
    problem_child.children[problem_child.cnt_keys] = right_bro.children[0]; // забираем ребенка с удаленного элемента

    if (!problem_child.is_leaf) {
        b_tree_node temp(problem_child.children[problem_child.cnt_keys]);
        temp.parent = problem_child.file_name;
        temp.write();
    }

    for (int i = 0; i < right_bro.cnt_keys; ++i) // чистим удаленные элементы
        right_bro.children[i] = right_bro.children[i + 1];
    for (int i = 0; i < right_bro.cnt_keys - 1; ++i) {
        right_bro.keys[i] = right_bro.keys[i + 1];
        right_bro.values[i] = right_bro.values[i + 1];
    }

    right_bro.cnt_keys--;
}

void b_tree::rebase_with_left(b_tree_node &parent, b_tree_node &problem_child,
                             b_tree_node &left_bro, int index) {
    for (int i = problem_child.cnt_keys; i > 0; --i) {
        problem_child.keys[i] = problem_child.keys[i - 1];
        problem_child.values[i] = problem_child.values[i - 1];
    }
    for (int i = (int(problem_child.cnt_keys)) + 1; i > 0; --i)
        problem_child.children[i] = problem_child.children[i - 1];
    b_tree_node::copy_key(problem_child, // переносим ключ-разделитель в проблемный нод
                          make_pair(parent.keys[index - 1],
                                    parent.values[index - 1]), 0);
    problem_child.cnt_keys++;
    b_tree_node::copy_key(parent, // переносим самый правый элемент левого брата на место разделителя
                          make_pair(left_bro.keys[left_bro.cnt_keys - 1],
                                    left_bro.values[left_bro.cnt_keys - 1]), index - 1);
    problem_child.children[0] =
            left_bro.children[left_bro.cnt_keys]; // забираем ребенка с удаленного элемента

    if (!problem_child.is_leaf) {
        b_tree_node temp(problem_child.children[0]);
        temp.parent = problem_child.file_name;

        temp.write();
    }
    left_bro.cnt_keys--;
}

void b_tree::rebase(b_tree_node& parent, b_tree_node& problem_child){
    auto iter = find(parent.children.begin(),
                     parent.children.begin() + parent.cnt_keys, problem_child.file_name);
    int index = iter - parent.children.begin();

    if (problem_child.cnt_keys == t - 1){
        if (parent.cnt_keys > index) {
            b_tree_node right_bro(parent.children[index + 1]);
            if (right_bro.cnt_keys >= t) { // проверка на возможность ребейза с правым братом
                rebase_with_right(parent, problem_child, right_bro, index);

                parent.write();
                problem_child.write();
                right_bro.write();

                root->read();
                return;
            }
        }

        if (index > 0){
            b_tree_node left_bro(parent.children[index - 1]);
            if (left_bro.cnt_keys >= t) { // проверка на возможность ребейза с левым братом
                rebase_with_left(parent, problem_child, left_bro, index);

                parent.write();
                problem_child.write();
                left_bro.write();

                root->read();
                return;
            }
        }
        // если ни один из ребейзов невозможен - делаем мердж
        if (parent.cnt_keys > index)
            merge(parent, index);
        else
            merge(parent, index - 1);

        problem_child.read();
    }
}

b_tree::b_tree() {
    b_tree_node node;

    node.is_leaf = true;
    node.write();

    root = make_unique<b_tree_node>(node);
}

[[nodiscard]] optional<pair<b_tree_node, size_t>> b_tree::search(int key) const {
    return search_nodes(*root, key);
}

bool b_tree::insert_nonfull(b_tree_node root, int key, int value){
    long i = long(root.cnt_keys) - 1;

    for (int j = 0; j < root.cnt_keys; ++j) {
        if (root.keys[j] == key)
            return false;
    }

    if (root.is_leaf) {
        while (i >= 0 && key < root.keys[i]) {
            root.keys[i + 1] = root.keys[i];
            root.values[i + 1] = root.values[i];
            i--;
        }

        b_tree_node::copy_key(root, make_pair(key, value), i + 1);

        root.cnt_keys++;
        root.write();

        return true;
    } else {
        while (i >= 0 && key < root.keys[i])
            i--;
        i++;
        b_tree_node temp(root.children[i]);

        for (int j = 0; j < temp.cnt_keys; ++j) {
            if (temp.keys[j] == key)
                return false;
        }

        if (temp.cnt_keys == 2*t - 1){
            b_tree_node::split_child(root, i, temp);
            if (key > root.keys[i])
                i++;
        }

        temp = b_tree_node(root.children[i]);
        return insert_nonfull(temp, key, value);
    }
}

bool b_tree::insert(int key, int value) {
    b_tree_node r = *root;

    if (r.cnt_keys == 2*t - 1) { // если корень полон - разбиваем его с помощью split_child
        b_tree_node s;
        root = make_unique<b_tree_node>(s);

        root->is_leaf = false;
        root->cnt_keys = 0;
        r.parent = root->file_name;
        root->children[0] = r.get_name();

        b_tree_node::split_child(*root, 0, r);

    }

    auto res = insert_nonfull(*root, key, value);
    root->read();
    return res;
}

string b_tree::remove(int key) {
    auto node_with_key = search(key);

    if (!node_with_key.has_value())
        return "null";

    b_tree_node node = node_with_key->first;
    string result;

    if (node.is_leaf)
        result = remove_in_leaf(node, key);
    else
        result = remove_in_nonleaf(node, key);


    return result;
}