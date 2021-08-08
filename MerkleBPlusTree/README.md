# Отчет
## Постановка задачи
Необходимо реализовать Merkle B+ Tree или же B+ дерево с возможностью сравнения любых поддеревьев с помощью рекурсивного подсчета хэша каждой вершины по значениям хэшей ее потомков.

## Описание
В качестве референса для реализации был выбран данный исходный код: https://github.com/begeekmyfriend/bplustree/tree/in-memory

Специально для реализации дерева Меркля был выбран референс код с имплементацией B+ дерева, хранящего данные в памяти, так как динамическое обновление хэша дерева оказалось неэффективным в связке с хранением данных на диске в силу необходимости добавления лишних операций чтения и записи при каждом изменении дерева.

Так как референсная реализация дерева представлена на языке `C`, в первую очередь был проведен глобальный рефакторинг кода, дабы соответствовать стандартам языка `C++`. В большинстве своем это коснулось измененной имплементации работы с сырой памятью, замены устаревших функций и обертки статических интерфейсов взаимодействия с деревом в привычный класс.

Также, для внесения адаптируемости структуры данных работа с конкретными ключами и данными (int и int) была заменена на шаблоны. Для корректной работы дерева тип ключа должен соответствовать следующим требованиям:
* Иметь перегруженные операции сравнения
* Иметь перегруженное приведение к строке

## Реализация
В первую очередь, в интерфейс структур вершин было добавлено поле для хранение хэша и реализованы две функции хэширования, для листовых и нелистовых вершин соответственно.

### Хэширование листовой вершины
``` C++
static inline void hash_leaf(bplus_leaf<K, V> *node) {
    CryptoPP::BLAKE2b blake_hsasher;
    std::string helper;

    for (int i = 0; i < node->count; ++i) {
        helper = std::to_string(node->key[i]) + 
                std::to_string(node->data[i]);
        unsigned char buf[helper.size()];
        strcpy((char*) buf, helper.c_str());

        blake_hasher.Update(buf, helper.size());
    }

    node->hash.resize(blake_hasher.DigestSize());
    blake_hasher.Final((unsigned char*) &node->hash[0]);
}
```
В данном фрагменте кода представлен метод хэширования листовой вершины дерева. Хэширование производится с помощью BLAKE2 функции хэширования, посредством формирования хэша вершины по данным, лежащим в ней. Для корректной работы функции хэширования, обновление хэша происходит с использованием строкового представления ключей и значений, чем и обосновывается условие обязательного переопределения приведения к строке в используемых типах ключей и значений. 

### Хэширование нелистовой вершины
``` C++
static inline void hash_non_leaf(bplus_non_leaf<K, V> *node) {
    CryptoPP::BLAKE2b blake_hasher;

    for (int i = 0; i < node->count; ++i) {
        unsigned char buf[node->sub_ptr[i]->hash.size()];
        strcpy((char*) buf, node->sub_ptr[i]->hash.c_str());

        blake_hasher.Update(buf, node->sub_ptr[i]->hash.size());
    }

    node->hash.resize(blake_hasher.DigestSize());
    blake_hasher.Final((unsigned char*) &node->hash[0]);
}
```
В данном фрагменте кода представлен метод хэширования нелистовой вершины дерева. Хэширование производится с помощью BLAKE2 функции хэширования, посредством формирования хэша вершины по хэшам дочерних вершин, лежащим в ней.

Хэширующей функцией выбран blake2 из-за того, что изменение одного бита данных приводит к изменению каждого бита хэша с 50% вероятностью, что позволяет сделать абсолютно любую функцию комбирирования хэшей дочерних вершин эффективной, ибо даже при незначительном изменении одной из вершин спровоцирует сильное изменение её хэш-значения.

Основная логика работы с деревом Меркля, а именно - обновление хэша всего поддерева при каком-либо изменении, была добавлена в основных изменяющих дерево операциях: добавление элемента в дерево,  разделение вершины при переполнении, удаление элемента из дерева, соединение вершин при недостатке элементов в вершинах. Такие сложности обусловлены желанием добиться логарифмической сложности любого обновления хэша дерева и константной сложности получения хэша любой известной пользователю вершины.

## Дополнительный функционал

В используемом референсе присутствовала реализация функции относительно удобного вывода всего дерева в консоль, что может быть крайне удобно для дебага. Для повышения гибкости, в реализацию функции вывода был добавлен вывод хэша каждой вершины перед перечислением ее ключей, что также может быть полезно при отладке.

Написана функция получения списка пар наиболее глубоких непохожих вершин двух деревьев. Уточнение про наиболее глубокие необходимо, ибо в случае деревьев разной размерности, алгоритм опустится до уровня листовых вершин наименьшего дерева и вернет пару с соответствующими им вершинами сравниваемого дерева.


``` C++
using change_set = std::vector<std::pair<bplus_node<K, V> *, bplus_node<K, V> *>>;

change_set get_change_set(bplus_tree<K, V> *other) {
    change_set different_nodes;
    if (root && other->root)
        get_nodes_diff(different_nodes, root, other->root);
    else if (root != other->root) {
        different_nodes.push_back(std::make_pair(root, other->root));
    }

    return different_nodes;
}
```
В данном фрагменте кода представлен метод реализующий получение списка пар наиболее глубоких непохожих вершин двух деревьев. В данном методе публичного интерфейса класса осуществляется проверка на наличие корней в двух деревьях и логика действий в различных случаях.

``` C++
static inline void get_nodes_diff(change_set &different_nodes,
                                      bplus_node<K, V> *f, bplus_node<K, V> *s) {
    if (*f == *s)
        return;
    if (is_leaf(f) || is_leaf(s)) {
        different_nodes.push_back(std::make_pair(f, s));
        return;
    }

    int n = std::min(f->count, s->count);

    for (int i = 0; i < n; ++i) {
        get_nodes_diff(different_nodes,
                        ((bplus_non_leaf<K, V> *)f)->sub_ptr[i],
                        ((bplus_non_leaf<K, V> *)s)->sub_ptr[i]);
    }

    if (f->count > s->count) {
        for (int i = n; i < f->count; ++i) {
            different_nodes.push_back(std::make_pair(f, nullptr));
        }
    } else {
        for (int i = n; i < s->count; ++i) {
            different_nodes.push_back(std::make_pair(nullptr, s));
        }
    }
}
```
В данном фрагменте кода представлен метод реализующий проверку схожести двух вершин. Если хотя бы одна из вершин является листовой и их хэши разнятся - происходит добавление пары в список непохожих вершин. В случае, если обе вершины являются нелистовыми, происходит рекурсивное сравнение их дочерних вершин, а также добавление в список непохожих пар с одним nullptr в случае, если количество ключей в вершинах не совпадает. Данный метод является приватным методом класса и вызывается только из метода `get_change_set`.

Для тестирования реализации были написаны примитивные юнит-тесты, проверяющие корректность дерева и его способность верно обновлять хэш.

### Тесты корректности добавления и удаления элементов из дерева
``` C++
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
```

### Тест хэш функции
``` C++
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
        tree_b->insert(i, i);
    }
    assert(tree_a->root->hash != tree_b->root->hash);

    delete tree_a;
    delete tree_b;
}
```

### Тест функции получения списка непохожих вершин
``` C++
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
```
