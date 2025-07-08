#pragma once

#include <vector>

#include "valueData.hh"

class Node {
public:
    Value val;
    Node* left;
    Node* right;
    int height;
    std::vector<int> ids;

    Node(Value inValue) :
        val(inValue), left(0), right(0), height(1) {}
};
