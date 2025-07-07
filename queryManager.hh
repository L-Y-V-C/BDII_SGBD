#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include "tree.hh"

class QueryManager
{
public:
    std::vector<std::string> tokens;
    std::string tableName = "";
    std::string typeToTree = "";
    std::string fieldToTree = "";
    std::vector<std::vector<std::string>> dataInfo;
    int treeType = 0;

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
        iss2 >> tableName;

        std::size_t wherePos = query.find("WHERE");
        if (wherePos != std::string::npos) {
            tokens.push_back("WHERE");
            std::string cond = query.substr(wherePos + 5); // skip "WHERE"
            cond = trim(cond);

            // Extraer campo, comparador y valor de forma robusta
            std::istringstream iss3(cond);
            std::string campo, comparador, valor;

            iss3 >> campo;
            iss3 >> comparador;

            char c;
            iss3 >> std::ws; // skip whitespace
            if (iss3.peek() == '"') {
                iss3.get(); // remove starting quote
                std::getline(iss3, valor, '"'); // read until closing quote
            }
            else {
                iss3 >> valor;
            }

            tokens.push_back(campo);
            tokens.push_back(comparador);
            tokens.push_back(valor);
        }
        for (auto i : tokens) {
            if (i == "WHERE") {
                checkType();
            }
        }
        if (typeToTree != "") {
            if (typeToTree == "INTEGER" || typeToTree == "BOOL" || typeToTree == "BOOLEAN")
                treeType = 0;
            else if (typeToTree == "DECIMAL")
                treeType = 1;
            else
                treeType = 2;
        }
        AVLTree tree(treeType);
    }
    void parseInsert(std::string query) {
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
    void createTree() {
        for (auto i : tokens) {
            if (i == "WHERE")
                printf("\nfind WHERE\n");
        }
    }
    void checkType() {
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i] == "WHERE") {
                fieldToTree = tokens[i + 1];
                break;
            }
        }
        for (auto i : dataInfo) {
            if (fieldToTree == i[0]) {
                typeToTree = i[1];
                break;
            }
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