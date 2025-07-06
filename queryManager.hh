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
    std::string tableName;

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
            std::cerr << "tipo de consulta no soportado\n";
        }
    }

    void printTokens() {
        std::cout << tableName << "\n";
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

        // extract fields
        std::string fields = query.substr(6, fromPos - 6);  // skip "SELECT"
        std::istringstream iss(fields);
        std::string field;
        while (std::getline(iss, field, ',')) {
            tokens.push_back(trim(field));
        }

        // extract table name
        std::string rest = query.substr(fromPos + 4); // skip "FROM"
        std::istringstream iss2(rest);
        std::string tableName;
        iss2 >> tableName;
        tokens.push_back(tableName);

        std::size_t wherePos = query.find("WHERE");
        if (wherePos != std::string::npos) {
            tokens.push_back("WHERE");
            std::string cond = query.substr(wherePos + 5); // skip "WHERE"
            cond = trim(cond);
            std::istringstream iss3(cond);
            std::string part;
            int count = 0;
            while (iss3 >> part && count < 3) {
                // quitar comillas si es texto
                if (!part.empty() && part.front() == '"' && part.back() == '"') {
                    part = part.substr(1, part.size() - 2);
                }
                tokens.push_back(part);
                count++;
            }
        }
    }



    void parseInsert(const std::string query) {
        tokens.push_back("INSERT");

        std::size_t intoPos = query.find("INTO");
        if (intoPos == std::string::npos)
            return;

        std::string rest = query.substr(intoPos + 4); // skip "INTO"
        std::istringstream iss(rest);
        iss >> tableName;

        std::size_t valStart = query.find("VALUES(");
        if (valStart == std::string::npos)
            return;

        valStart += 7; // skip "VALUES("
        std::size_t valEnd = query.find(')', valStart);
        if (valEnd == std::string::npos)
            return;

        std::string values = query.substr(valStart, valEnd - valStart);
        std::istringstream iss2(values);
        std::string value;
        while (std::getline(iss2, value, ',')) {
            value = trim(value);
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
std::cout << "------\n";
std::string q4 = "SELECT * FROM tabla WHERE age >= 18";
qm.parseQuery(q4);
qm.printTokens();
std::cout << "------\n";
std::string q5 = "SELECT name, id FROM tabla WHERE name = \"Juan\"";
qm.parseQuery(q5);
qm.printTokens();
*/

#endif