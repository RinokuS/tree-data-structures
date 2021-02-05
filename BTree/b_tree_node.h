#include <string>
#include <vector>

unsigned short t = 1; // переменная t для дерева
std::string bin_files_path; // путь к папке с бинарными файлами

class b_tree_node{
public:
    std::string file_name;
    size_t cnt_keys;
    bool is_leaf;
    std::vector<int> keys;
    std::vector<int> values;
    std::string parent;
    std::vector<std::string> children;

    b_tree_node();

    explicit b_tree_node(const std::string &fn);

    /// Метод для разделения полного ребенка с неполным родителем на 2 нода
    /// \param x родитель
    /// \param i индекс разделителя
    /// \param y ребенок
    static void split_child(b_tree_node& x, long i, b_tree_node& y);

    /// метод для чтения бинарных файлов
    void read();

    /// метод для записи нодов в бинарные файлы
    void write() const;

    [[nodiscard]] std::string get_name() const;

    /// метод для копирования ключа в ноду
    /// (чтобы постоянно не вставлять ключ и за ним значение по одному и тому же индексу)
    /// \param dest нода для вставки
    /// \param obj пара (ключ, значение)
    /// \param ind_dest индекс вставки
    static void copy_key(b_tree_node &dest, std::pair<int, int> obj, size_t ind_dest);

    /// метод для копирования диапазона ключей в ноду
    /// \param dest нода для вставки
    /// \param obj нода для копирования
    /// \param ind_dest индекс вставки
    /// \param ind_obj индекс начала диапазона
    /// \param size размер диапазона
    static void copy_keys(b_tree_node &dest, b_tree_node &obj,
                          size_t ind_dest, size_t ind_obj, size_t size);
};