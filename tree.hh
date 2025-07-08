#pragma once

#include <iostream>
#include <string>

#include "node.hh"
#include <iomanip>

class AVLTree {
public:
    Node* root;
    int compType; /*0 int - 1 float - 2 string*/

    AVLTree(int in_type) : root(0), compType(in_type) {}

    int height(Node* node) {
        if (!node)
            return 0;
        return node->height;
    }

    int balanceFactor(Node* node) {
        if (node == nullptr)
            return 0;
        return height(node->left) - height(node->right);
    }

    Node* rightRotate(Node* y) {
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

    Node* leftRotate(Node* x) {
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
    void myTrim(std::string& s) {
        if (s.empty()) return;
        // remove all \0
        s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
        if (s.empty())
            return;
        const std::string whitespace = " \t\n\r\f\v";
        // Trim right
        size_t end = s.find_last_not_of(whitespace);
        if (end != std::string::npos)
            s.erase(end + 1);
        else {
            s.clear();
            return;
        }
        // Trim left
        size_t start = s.find_first_not_of(whitespace);
        if (start != std::string::npos)
            s.erase(0, start);
        else {
            s.clear();
        }
    }
    int compare(Node* node, Value inValue) {
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
            myTrim(inValue.datString);
            myTrim(node->val.datString);
            if (inValue.datString < node->val.datString)
                return 0; 
            else if (inValue.datString > node->val.datString)
                return 1;
            else
                return 2;
            break;
        }
    }

    Node* insert(Node* node, Value inValue, int id) {
        if (!node) {
            Node* newNode = new Node(inValue);
            newNode->ids.push_back(id);
            return newNode;
            //return new Node(inValue);
        }
        if (compare(node, inValue) == 0)
            node->left = insert(node->left, inValue, id);
        else if (compare(node, inValue) == 1)
            node->right = insert(node->right, inValue, id);
        else if (compare(node, inValue) == 2) {
            node->ids.push_back(id);
            return node;
        }
        
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

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left)
            current = current->left;
        return current;
    }

    Node* find(Node* node, Value inValue) {
        if (node == nullptr)
            return nullptr;
        if (compare(node, inValue) == 2)
            return node;
        if (compare(node, inValue) == 0)
            return find(node->left, inValue);
        return find(node->right, inValue);
    }

    bool find(Value inValue) {
        return find(root, inValue);
    }

    std::vector<int> returnIds(std::string data) {
        Value v;
        std::vector<int> result;
        switch (compType) {
        case 0:
            v.datInt = std::stoi(data);
            break;
        case 1:
            v.datFloat = std::stof(data);
            break;
        case 2:
            v.datString = data;
            break;
        }
        Node* node = find(root, v);
        if (node)
            result = node->ids; 
        return result;
    }

    void insertFromVector(std::vector<std::pair<std::string, int>> dataToTree) {
        for (auto p : dataToTree) {
            Value v;
            switch (compType) {
            case 0:
                v.datInt = std::stoi(p.first);
                break;
            case 1:
                v.datFloat = std::stof(p.first);
                break;
            case 2:
                v.datString = p.first;
                break;
            }
            insertWithId(v, p.second);
        }
    }

    void insertWithId(Value inValue, int id) {
        root = insert(root, inValue, id);
    }

    void printTree(Node* node, int space = 0) {
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
};