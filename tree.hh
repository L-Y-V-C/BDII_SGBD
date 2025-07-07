#pragma once

#include <iostream>
#include <string>

#include "node.hh"

class AVLTree {
public:
    Node* root;
    int compType; /*0 int - 1 float - 2 string*/

    AVLTree(int in_type) : root(0), compType(in_type) {}
    int height(Node* node);
    int balanceFactor(Node* node);
    Node* rightRotate(Node* y);
    Node* leftRotate(Node* x);
    Node* insert(Node* node, Value inValue);
    Node* minValueNode(Node* node);
    bool find(Node* root, Value inValue);
    void insert(Value inValue);
    bool find(Value inValue);
    int compare(Node* node, Value inValue);
    void printTree(Node* node, int space);
};

int AVLTree::height(Node* node) {
    if (!node)
        return 0;
    return node->height;
}

int AVLTree::balanceFactor(Node* node) {
    if (node == nullptr)
        return 0;
    return height(node->left) - height(node->right);
}

Node* AVLTree::rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    /*rotation*/
    x->right = y;
    y->left = T2;
    /*height fix*/
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;/*new root*/
}

Node* AVLTree::leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    /*rotation*/
    y->left = x;
    x->right = T2;
    /*height fix*/
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;/*new root*/
}
int AVLTree::compare(Node* node, Value inValue) {
    switch (compType) {
    case 0:
        if (inValue.datInt < node->val.datInt)
            return 0;
        else if (inValue.datInt > node->val.datInt)
            return 1;
        else
            return 2;
        break;
    case 1:
        if (inValue.datFloat < node->val.datFloat)
            return 0;
        else if (inValue.datFloat > node->val.datFloat)
            return 1;
        else
            return 2;
        break;
    case 2:
        if (inValue.datString < node->val.datString)
            return 0;
        else if (inValue.datString > node->val.datString)
            return 1;
        else
            return 2;
        break;
    }
}

Node* AVLTree::insert(Node* node, Value inValue) {
    if (!node)
        return new Node(inValue);
    if (compare(node, inValue) == 0)
        node->left = insert(node->left, inValue);
    else if (compare(node, inValue) == 1)
        node->right = insert(node->right, inValue);
    else if (compare(node, inValue) == 2)
        return node;
    /*height ancestor update*/
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);
    /*cases*/
    //LL
    if (balance > 1 && compare(node->left, inValue) == 0)
        return rightRotate(node);
    //RR
    if (balance < -1 && compare(node->right, inValue) == 1)
        return leftRotate(node);
    //LR
    if (balance > 1 && compare(node->left, inValue) == 1) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    //RL
    if (balance < -1 && compare(node->right, inValue) == 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Node* AVLTree::minValueNode(Node* node) {
    Node* current = node;
    while (current->left)
        current = current->left;
    return current;
}

bool AVLTree::find(Node* root, Value inValue) {
    if (root == nullptr)
        return false;
    if (compare(root, inValue) == 2)
        return true;
    if (compare(root, inValue) == 0)
        return find(root->left, inValue);
    return find(root->right, inValue);
}

void AVLTree::insert(Value inValue) {
    root = insert(root, inValue);
}

bool AVLTree::find(Value inValue) {
    return find(root, inValue);
}

void AVLTree::printTree(Node* node, int space = 0) {
    if (node == nullptr)
        return;
    space += 4;
    printTree(node->right, space);
    std::cout << std::endl;
    for (int i = 4; i < space; i++)
        std::cout << " ";
    switch (compType) {
    case 0:
        std::cout << node->val.datInt;
        break;
    case 1:
        std::cout << node->val.datFloat;
        break;
    case 2:
        std::cout << node->val.datString;
        break;
    }
    std::cout << std::endl;
    printTree(node->left, space);
}