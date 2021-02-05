#include <iostream>
#include <fstream>

#include "gdal_priv.h"
#include "gdal.h"
#include "ogrsf_frmts.h"
#include "boost/geometry.hpp"

using namespace std;
// сокращения для удобства
using point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using box = boost::geometry::model::box<point>;

/// Метод для получения минимального обрамляющего прямоугольника по полигону
/// \param polygon заданный полигон
/// \return MBR полигона
box get_MBR(OGRPolygon* polygon) {
    const shared_ptr<OGREnvelope> polygon1Envelope(new OGREnvelope);
    polygon->getEnvelope(polygon1Envelope.get()); // используем функцию с семинаров, чтобы достать граничные точки
    // нам достаточно левой нижней и правой верхней точек для того, чтобы задать прямоугольник
    auto left_b = point(polygon1Envelope->MinX, polygon1Envelope->MinY);
    auto right_u = point(polygon1Envelope->MaxX, polygon1Envelope->MaxY);

    return box(left_b, right_u);
}

/// Метод для получения заполненного датасетом дерева
/// \return заполненное дерево (auto ибо тип громоздкий даже с сокращениями,
/// а новое вводить ради одной строки не хочется)
auto get_filled_tree(const char* data_path) {
    auto rtree = boost::geometry::index::rtree< // создаем пустое дерево
                 std::pair<box, int>,
    boost::geometry::index::quadratic<8, 4>>();

    auto dataset = static_cast<GDALDataset*>(GDALOpenEx( // открываем датасет, способом с семинара
            data_path,
            GDAL_OF_VECTOR,
            nullptr, nullptr, nullptr));

    for (auto &&layer: dataset->GetLayers()) { // проходимся по каждому слою
        for (auto &&feature: layer) { // из каждого слоя достаем все объекты
            auto* geometry = feature->GetGeometryRef();
            int id = feature->GetFieldAsInteger(0);

            rtree.insert(make_pair(get_MBR(geometry->toPolygon()), id)); // каждый из объектов инсертим в дерево
        }
    }

    return rtree; // возвращаем заполненное дерево
}

int main(int argc, char* argv[]) {
    GDALAllRegister();
    auto rtree = get_filled_tree(argv[1]);
    ifstream is{argv[2]};
    ofstream os{argv[3]};

    double x_min, y_min, x_max, y_max;
    is >> x_min >> y_min >> x_max >> y_max; // считываем координаты из файла
    vector<std::pair<box, int>> res; // создаем вектор для результата (для query)

    auto rect = box(point(x_min, y_min),point(x_max, y_max));
    rtree.query(boost::geometry::index::intersects(rect), back_inserter(res));

    sort(res.begin(), res.end(), [](std::pair<box, int> f, std::pair<box, int> s) {
        return f.second < s.second; // сортируем все ответы по id в порядке возврастания
    });

    for (const auto& resu: res)
        os << resu.second << "\n";

    return 0;
}
