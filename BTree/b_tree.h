#include <optional>
#include "b_tree_node.h"

class b_tree {
private:
    std::unique_ptr<b_tree_node> root = nullptr;

    /// метод поиска значения в ноде
    /// \param semi_root корень поддерева, в котором идет поиск
    /// \param key ключ для поиска
    /// \return возвращаем optional, ибо ключ может не найтись
    static std::optional<std::pair<b_tree_node, size_t>> search_nodes(b_tree_node semi_root, int key);

    /// метод удаления элемента в листке с >= t элементов (непустом листке)
    /// \param node нода
    /// \param key ключ для удаления
    /// \return удаленное значение
    static std::string remove_in_good_leaf(b_tree_node &node, int key);

    /// метод для удаления элемента из листка (любого)
    /// \param node нода
    /// \param key ключ для удаления
    /// \return удаленное значение
    std::string remove_in_leaf(b_tree_node &node, int key);

    /// метод для удаления элемента из непустой нелистовой ноды
    /// \param node нода
    /// \param key ключ для удаления
    /// \return удаленное значение
    std::string remove_in_good_nonleaf(b_tree_node &node, int key);

    /// метод для удаления элемента из нелистовой ноды
    /// \param node нода
    /// \param key ключ для удаления
    /// \return удаленное значение
    std::string remove_in_nonleaf(b_tree_node &node, int key);

    /// метод для мерджа двух пустых нод (по t-1 элементу) между собой
    /// \param parent родитель двух нод
    /// \param index_of_link индекс соединяющего элемента
    void merge(b_tree_node &parent, int index_of_link);

    /// метод для ребейза с использованием правого брата
    /// \param parent родитель
    /// \param problem_child пустой ребенок
    /// \param right_bro правый брат
    /// \param index индекс связующего элемента
    static void rebase_with_right(b_tree_node &parent, b_tree_node &problem_child,
                                  b_tree_node &right_bro, int index);

    /// метод для ребейза с использованием левого брата
    /// \param parent родитель
    /// \param problem_child пустой ребенок
    /// \param left_bro левый брат
    /// \param index
    static void rebase_with_left(b_tree_node &parent, b_tree_node &problem_child,
                                 b_tree_node &left_bro, int index);

    /// метод ребейза
    /// \param parent родитель
    /// \param problem_child пустой ребенок
    void rebase(b_tree_node& parent, b_tree_node& problem_child);

public:
    b_tree();

    ~b_tree() = default;

    [[nodiscard]] std::optional<std::pair<b_tree_node, size_t>> search(int key) const;

    /// метод для вставки элемента в неполную ноду
    /// \param root нода
    /// \param key ключ
    /// \param value значение
    /// \return
    static bool insert_nonfull(b_tree_node root, int key, int value);

    /// метод для добавления элемента
    /// \param key ключ
    /// \param value значение
    /// \return был ли элемент до этого
    bool insert(int key, int value);

    std::string remove(int key);
};