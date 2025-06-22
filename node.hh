#pragma once

template<class T>
struct Nodo {
    T data;
    Nodo<T>* nodos[2];
    int height;

    Nodo(const T& val) : data(val), height(1) {
        nodos[0] = nodos[1] = nullptr;
    }
};