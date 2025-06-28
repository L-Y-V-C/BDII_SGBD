#pragma once

#include <string>
#include <vector>

class Register
{
public:
    std::vector<std::string> fields;

    Register(std::vector<std::string> data) : fields(data) {}
};
