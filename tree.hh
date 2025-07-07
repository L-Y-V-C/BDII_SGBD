#pragma once

#include "node.hh"
#include <iomanip>

template<class T>
struct AVLTree {
    Nodo<T>* root = nullptr;
    int field = 0;

    std::string convertLowercast(const std::string& s) {
        std::string res = s;
        for (char& c : res)
            c = std::tolower(static_cast<unsigned char>(c));
        return res;
    }

    std::string convert(const T& data)
    {
        if (field == 2 || field == 3 || field == 4)
        {
            float valor = std::stof(data[field]);
            std::ostringstream ss;
            ss << std::setw(10) << std::setfill('0') << std::fixed << std::setprecision(2) << valor;
            return ss.str();
        }
        if (field == 1)
            return convertLowercast(data[field]);
        return data[field];
    }

    int height(Nodo<T>* n) {
        return n ? n->height : 0;
    }

    int balanceFactor(Nodo<T>* n) {
        return n ? height(n->nodos[1]) - height(n->nodos[0]) : 0;
    }

    bool sort(int num) {
        return num == 2 || num == -2;
    }

    void rrRotate(Nodo<T>** a_temp) {
        Nodo<T>* b_temp = (*a_temp)->nodos[1];

        (*a_temp)->height -= 2;

        (*a_temp)->nodos[1] = b_temp->nodos[0];
        b_temp->nodos[0] = *a_temp;

        *a_temp = b_temp;
    }

    void rlRotate(Nodo<T>** a_temp) {
        Nodo<T>* c_temp = (*a_temp)->nodos[1];
        Nodo<T>* b_temp = c_temp->nodos[0];

        (*a_temp)->height -= 2;
        b_temp->height += 1;
        c_temp->height -= 1;

        (*a_temp)->nodos[1] = b_temp->nodos[0];
        c_temp->nodos[0] = b_temp->nodos[1];
        b_temp->nodos[0] = *a_temp;
        b_temp->nodos[1] = c_temp;

        *a_temp = b_temp;
    }

    void llRotate(Nodo<T>** c_temp) {
        Nodo<T>* b_temp = (*c_temp)->nodos[0];

        (*c_temp)->height -= 2;

        (*c_temp)->nodos[0] = b_temp->nodos[1];
        b_temp->nodos[1] = (*c_temp);

        *c_temp = b_temp;
    }

    void lrRotate(Nodo<T>** c_temp) {
        Nodo<T>* a_temp = (*c_temp)->nodos[0];
        Nodo<T>* b_temp = a_temp->nodos[1];

        (*c_temp)->height -= 2;
        b_temp->height += 1;
        a_temp->height -= 1;

        a_temp->nodos[1] = b_temp->nodos[0];
        (*c_temp)->nodos[0] = b_temp->nodos[1];
        b_temp->nodos[0] = a_temp;
        b_temp->nodos[1] = *c_temp;

        *c_temp = b_temp;
    }

    Nodo<T>* insert(const T& val, Nodo<T>** n) {
        std::string clave = convert(val);

        if (!root) {
            root = new Nodo<T>(clave, val);
            return root;
        }

        if (!(*n)) return new Nodo<T>(clave, val);

        if (clave < (*n)->clave)
            (*n)->nodos[0] = insert(val, &((*n)->nodos[0]));
        else if (clave > (*n)->clave)
            (*n)->nodos[1] = insert(val, &((*n)->nodos[1]));
        else {
            (*n)->registros.push_back(val);
            return *n;
        }

        (*n)->height = std::max(height((*n)->nodos[0]), height((*n)->nodos[1])) + 1;
        int diff_height = balanceFactor(*n);

        if (sort(diff_height)) {
            if (diff_height < 0) {
                if ((*n)->nodos[0] && clave < (*n)->nodos[0]->clave)
                    llRotate(n);
                else
                    lrRotate(n);
            }
            else {
                if ((*n)->nodos[1] && clave > (*n)->nodos[1]->clave)
                    rrRotate(n);
                else
                    rlRotate(n);
            }
        }

        return *n;
    }

    void inorder(Nodo<T>* n) {
        if (!n) return;
        inorder(n->nodos[0]);
        for (const auto& r : n->registros)
            std::cout << r[field] << "   |  ";
        inorder(n->nodos[1]);
    }

    void inorder2(Nodo<T>* n) {
        if (!n) return;
        inorder2(n->nodos[0]);
        for (const auto& r : n->registros)
            std::cout << "[" << r[0] << "] " << r[1] << " = " << r[field] << "\n";
        inorder2(n->nodos[1]);
    }
    void mostrarRegistrosPorNodo(Nodo<T>* n) {
        if (!n) return;
        mostrarRegistrosPorNodo(n->nodos[0]);

        std::cout << "Clave: " << n->clave << "\n";
        for (const auto& r : n->registros) {
            std::cout << "  [";
            for (size_t i = 0; i < r.size(); ++i) {
                std::cout << r[i];
                if (i < r.size() - 1) std::cout << ", ";
            }
            std::cout << "]\n";
        }
        std::cout << "\n";

        mostrarRegistrosPorNodo(n->nodos[1]);
    }


    bool find(const T& val, Node**& pPos) {
        pPos = &root;
        while (*pPos && (*pPos)->data != val) {
            pPos = &((*pPos)->nodos[(*pPos)->data < val]);
        }
        return *pPos != 0;
    }
};