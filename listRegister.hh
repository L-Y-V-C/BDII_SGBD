#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Register.hh"

class ListRegister
{
public:
    ListRegister(std::vector<std::string> in_fieldNames) : fieldNames(in_fieldNames) {}

    void add(std::vector<std::string> data)
    {
            LR.push_back(Register(data));
    }

    size_t size() { return LR.size(); }

    void print()
    {
        for (std::string name : fieldNames) // Encabezados
            std::cout << name << "\t\t";
        std::cout << "\n";

        for (Register& reg : LR)
        {
            for (std::string& val : reg.fields)
                std::cout << val << "\t\t";
            std::cout << "\n";
        }
    }


    ////////////////////


    Register& operator[](int index) { return LR[index]; }

    
    const std::vector<std::string>& getFieldNames() const { return fieldNames; }

private:
    std::vector<Register> LR;
    std::vector<std::string> fieldNames;

};
