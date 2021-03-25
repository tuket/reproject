#pragma once

namespace tl
{

template <typename T>
class TableView
{
public:
    TableView() : w(0), h(0), data(nullptr) {}
    TableView(int w, int h, T* data) : w(w), h(h), data(data) {}

    const T& operator()(int x, int y)const;
    T& operator()(int x, int y);

    const T* operator[](int y)const;
    T* operator[](int y);

    int width()const { return w; }
    int height()const { return h; }

protected:
    int w, h;
    T* data;
};

template <typename T>
class Table : public TableView<T>
{
public:
    Table() : TableView<T>() {}
    Table(int w, int h) : TableView<T>(w, h, new T[w*h]) {}
    ~Table() { delete[] TableView<T>::data; }
};

template <typename T>
using CTable = TableView<const T>;

// ----------------------------------------------------------------------------------------------------

template <typename T>
const T& TableView<T>::operator()(int x, int y)const {
    assert(x >= 0 && x < w && y >= 0 && y < h);
    return data[x + y*w];
}

template <typename T>
T& TableView<T>::operator()(int x, int y) {
    assert(x >= 0 && x < w && y >= 0 && y < h);
    return data[x + y*w];
}

template <typename T>
const T* TableView<T>::operator[](int y)const {
    assert(y >= 0 && y < h);
    return data + w*y;
}

template <typename T>
T* TableView<T>::operator[](int y) {
    assert(y >= 0 && y < h);
    return data + w*y;
}

}
