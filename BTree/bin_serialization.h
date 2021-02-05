#include <iostream>
#include <string>
#include <vector>

namespace bin_serialization {
    /// сериализация одного элемента
    /// \tparam T тип элемента
    /// \param pod элемент
    /// \param out поток
    template <typename T>
    void serialize(T pod, std::ostream& out) {
        out.write(reinterpret_cast<const char*>(&pod), sizeof(pod));
    }

    /// сериализация строки
    /// \param str строка
    /// \param out поток
    void serialize(const std::string& str, std::ostream& out) {
        serialize(str.size(), out);
        out.write(reinterpret_cast<const char*>(str.data()), str.size());
    }

    /// сериализация вектора
    /// \tparam T тип элементов
    /// \param data вектор
    /// \param out поток
    template <typename T>
    void serialize(const std::vector<T>& data, std::ostream& out) {
        serialize(data.size(), out);
        for (const auto& elem : data) {
            Serialize(elem, out);
        }
    }

    /// десериализация элемента
    /// \tparam T тип элемента
    /// \param in поток
    /// \param pod ссылка на элемент для записи
    template <typename T>
    void deserialize(std::istream& in, T& pod) {
        in.read(reinterpret_cast<char*>(&pod), sizeof(pod));
    }

    /// десериализация строки
    /// \param in поток
    /// \param str ссылка на строку для записи
    void deserialize(std::istream& in, std::string& str) {
        size_t size;
        deserialize(in, size);
        str.resize(size);
        if (size > 0) {
            in.read(reinterpret_cast<char*>(&str[0]), size);
        }
    }

    /// десериализация вектора
    /// \tparam T тип элементов
    /// \param in поток
    /// \param data ссылка на вектор для записи
    template <typename T>
    void deserialize(std::istream& in, std::vector<T>& data) {
        size_t size;
        deserialize(in, size);
        data.clear();
        data.reserve(size);
        for (size_t i = 0; i != size; ++i) {
            T elem;
            Deserialize(in, elem);
            data.push_back(std::move(elem));
        }
    }
}
