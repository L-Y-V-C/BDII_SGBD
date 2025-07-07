#pragma once
#include <string>
#include <vector>
#include <iomanip>

std::vector<std::string> separate_string(std::string& in_str, char separator)
{
    std::vector<std::string> result;

    std::string current;
    for (char c : in_str)
    {
        if (c == separator)
        {
            result.push_back(current);
            current.clear();
        }
        else
            current += c;
    }
    result.push_back(current);

    return result;
}

std::string clean_string(const std::string& str)
{
    std::string cleaned;
    for (char c : str)
    {
        if (c != '\0') cleaned += c;
    }
    return cleaned;
}

void print_table(const std::vector<std::vector<std::string>>& table, int column_width = 15)
{

    for (auto& row : table)
    {
        for (auto& cell : row)
        {
            std::string cleaned = clean_string(cell);
            std::cout << std::left << std::setw(column_width) << cleaned;
        }
        std::cout << '\n';
    }
}