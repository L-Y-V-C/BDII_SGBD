#pragma once
#include "node.hh"

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

    int height(Nodo<T>* n) {
        if (!n) return 0;
        return n->height;
    }

    int balanceFactor(Nodo<T>* n) {
        if (!n) return 0;
        return height(n->nodos[1]) - height(n->nodos[0]);
    }

    bool sort(int num) {
        return num == 2 || num == -2;
    }

    bool compareByField(const T& a, const T& b) {
        std::string aa = a[field];
        std::string bb = b[field];
        if (field == 1 || field == 2) {
            aa = convertLowercast(aa);
            bb = convertLowercast(bb);
        }
        return aa <= bb;
    }

    void rrRotate(Nodo<T>** a_temp) {
        Nodo<T>* b_temp = (*a_temp)->nodos[1];
        Nodo<T>* c_temp = (b_temp)->nodos[1];

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
        Nodo<T>* a_temp = b_temp->nodos[0];

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
        if (!root) {
            root = new Nodo<T>(val);
            return root;
        }

        if (!(*n)) return new Nodo<T>(val);
        if (compareByField(val, (*n)->data))
            (*n)->nodos[0] = insert(val, &((*n)->nodos[0]));
        else if (compareByField((*n)->data, val))
            (*n)->nodos[1] = insert(val, &((*n)->nodos[1]));
        else
            return *n;

        (*n)->height = std::max(height((*n)->nodos[0]), height((*n)->nodos[1])) + 1;
        int diff_height = balanceFactor(*n);

        if (sort(diff_height)) {
            if (diff_height < 0) {
                if ((*n)->nodos[0] && compareByField((*n)->nodos[0]->data, val))
                    lrRotate(n);
                else
                    llRotate(n);
            }
            else {
                if ((*n)->nodos[1] && compareByField(val, (*n)->nodos[1]->data))
                    rlRotate(n);
                else
                    rrRotate(n);
            }
        }
        return *n;
    }

    void preorder(Nodo<T>* n) {
        if (!n) return;
        std::cout << n->data[field] << " ";
        preorder(n->nodos[0]);
        preorder(n->nodos[1]);
    }
};