#pragma once

template<class T>
struct Nodo {
    std::string clave; 
    std::vector<T> registros;
    Nodo* nodos[2];
    int height;

    Nodo(const std::string& clave_, const T& val) : clave(clave_), height(1) {
        registros.push_back(val);
        nodos[0] = nodos[1] = nullptr;
    }
};