#ifndef QUERYMANAGER_HH
#define QUERYMANAGER_HH

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

class QueryManager
{
public:
    std::vector<std::string> tokens;

    void parseQuery(std::string query) {
        tokens.clear();

        std::string trimmed = trim(query);
        if (startsWith(trimmed, "SELECT")) {
            parseSelect(trimmed);
        }
        else if (startsWith(trimmed, "INSERT")) {
            parseInsert(trimmed);
        }
        else {
            std::cerr << "Unsupported query type.\n";
        }
    }

    void printTokens() {
        for (std::string t : tokens) {
            std::cout << t << "\n";
        }
    }

private:
    std::string trim(std::string& str) {
        std::size_t first = str.find_first_not_of(" \t\n\r");
        std::size_t last = str.find_last_not_of(" \t\n\r");
        return (first == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    bool startsWith(std::string& str, std::string prefix) {
        return str.size() >= prefix.size() &&
            std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    void parseSelect(std::string query) {
        tokens.push_back("SELECT");

        std::size_t fromPos = query.find("FROM");
        if (fromPos == std::string::npos)
            return;
        std::string fields = query.substr(6, fromPos - 6);  // skip "SELECT"
        std::istringstream iss(fields);
        std::string field;
        while (std::getline(iss, field, ',')) {
            tokens.push_back(trim(field));
        }
    }

    void parseInsert(const std::string query) {
        tokens.push_back("INSERT");

        std::size_t valStart = query.find("VALUES(");
        if (valStart == std::string::npos) return;

        valStart += 7; // skip "VALUES("
        std::size_t valEnd = query.find(')', valStart);
        if (valEnd == std::string::npos)
            return;

        std::string values = query.substr(valStart, valEnd - valStart);
        std::istringstream iss(values);
        std::string value;
        while (std::getline(iss, value, ',')) {
            value = trim(value);
            // Remove quotes if present
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            tokens.push_back(value);
        }
    }
};
/*
QueryManager qm;

std::string q1 = "SELECT * FROM tabla";
qm.parseQuery(q1);
qm.printTokens();

std::cout << "------\n";

std::string q2 = "SELECT name, id FROM tabla";
qm.parseQuery(q2);
qm.printTokens();

std::cout << "------\n";

std::string q3 = "INSERT INTO tabla VALUES(\"id\", \"name\", \"ape\")";
qm.parseQuery(q3);
qm.printTokens();
*/

#endif