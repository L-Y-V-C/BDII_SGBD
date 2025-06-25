#pragma once

#include <vector>
#include <string>

class DataStruct {
public:
    std::string tableName;
    std::vector<std::string> field;
    std::vector<int> number;
    DataStruct() {}
};