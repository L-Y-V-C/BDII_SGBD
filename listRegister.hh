#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Register.hh"

class ListRegister {
private:
    std::vector<Register> LR;
    std::vector<std::string> fieldNames;
    

public:
    ListRegister(std::vector<std::string> in_fieldNames) : fieldNames(in_fieldNames) {}

    void add(std::vector<std::string> data) {   /////////////////////// modificar segun ustedes :p //////////////////////////// 
        LR.emplace_back(data);
    }

    size_t size() {
        return LR.size();
    }

    void print() {
        for (std::string name : fieldNames) { // encabezados
            std::cout << name << "\t\t";
        }
        std::cout << "\n";

        for (Register& reg : LR) {
            for (std::string& val : reg.fields) {
                std::cout << val << "\t\t";
            }
            std::cout << "\n";
        }
    }


    ////////////////////


    Register& operator[](int index) {
        return LR[index];
    }

    const Register& operator[](int index) const {
        return LR[index];
    }
    
    const std::vector<std::string>& getFieldNames() const {
        return fieldNames;
    }


};
