#include <iostream>
#include <fstream>
#include <string>

#include "b_tree.h"

using namespace std;

int main(int argc, char* argv[]) {
    t = stoi(argv[1]);

    if (t >= 2) { // минимальное возможное t -- 2
        bin_files_path = argv[2];
        b_tree tree;
        ifstream is{argv[3]};
        ofstream os{argv[4]};

        string command;
        string str;
        int key, value;

        while (!is.eof()) { // выводим как в файл, так и в консоль для удобства :)
            command.clear();
            is >> command;
            if (command == "insert") {
                is >> key >> value;
                str = tree.insert(key, value) ? "true" : "false";
                os << str << "\n";
                cout << str << "\n";
            } else if (command == "find") {
                is >> key;
                auto res = tree.search(key);
                if (res.has_value()) {
                    str = to_string(res.value().first.values[res.value().second]);
                    os << str << "\n";
                    cout << str << "\n";
                } else {
                    str = "null";
                    os << str << "\n";
                    cout << str << "\n";
                }
            } else if (command == "delete") {
                is >> key;
                str = tree.remove(key);
                os << str << "\n";
                cout << str << "\n";
            }
        }
    }
    return 0;
}
