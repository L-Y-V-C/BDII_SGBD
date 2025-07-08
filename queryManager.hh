#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <utility>

#include "tree.hh"

class QueryManager
{
public:
    std::string typeQuery = "";
    std::vector<std::string> tokens1;
    std::vector<std::string> tokens2;
    std::string tableName = "";
    std::string typeToTree = "";
    std::string fieldToTree = "";
    std::vector<std::pair<std::string, int>> dataToTree;
    std::vector<std::vector<std::string>> dataInfo;
    std::vector<std::vector<std::string>> fieldsInfo;
    std::vector<int> idsQueryResult;
    int treeType = 0;
    std::vector<std::string> finalFields;
    std::vector<std::vector<std::string>> finalRegs;

    void parseQuery(std::string query) {
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
        std::cout << "tablename: " << tableName <<
            "\ntypeQuery: " << typeQuery << "\ntokens 1:\n";
        for (std::string t : tokens1) {
            std::cout << t << " ";
        }
        std::cout << "\ntokens 2:\n";
        for (std::string t : tokens2) {
            std::cout << t << " ";
        }
        printf("\n");
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
        typeQuery = "SELECT";

        std::size_t fromPos = query.find("FROM");
        if (fromPos == std::string::npos)
            return;

        // extract fields
        std::string fields = query.substr(6, fromPos - 6);  // skip "SELECT"
        std::istringstream iss(fields);
        std::string field;
        while (std::getline(iss, field, ',')) {
            tokens1.push_back(trim(field));
        }

        // extract table name
        std::string rest = query.substr(fromPos + 4); // skip "FROM"
        std::istringstream iss2(rest);
        iss2 >> tableName;

        std::size_t wherePos = query.find("WHERE");
        if (wherePos != std::string::npos) {
            tokens2.push_back("WHERE");
            std::string cond = query.substr(wherePos + 5); // skip "WHERE"
            cond = trim(cond);

            // extract after WHERE
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

            tokens2.push_back(campo);
            tokens2.push_back(comparador);
            tokens2.push_back(valor);
        }
        if (!tokens2.empty()) {
            for (auto i : tokens2) {
                if (i == "WHERE") {
                    checkType();
                }
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
        if (tokens2.empty()) {
            tokens2.push_back("");
            tokens2.push_back(dataInfo[0][0]);
        }

        for (int i = 0; i < fieldsInfo.size(); i++) {
            for (int j = 0; j < fieldsInfo[i].size(); j++) {
                if (tokens2.size() > 1 && tokens2[1] == dataInfo[j][0]) {
                    int id = std::stoi(fieldsInfo[i][0]);
                    dataToTree.push_back({ fieldsInfo[i][j], id });
                }
            }
        }
        tree.insertFromVector(dataToTree);
        idsQueryResult = tree.returnIds(tokens2.back());
        //tree.printTree(tree.root);
    }
    void myTrim(std::string& s) {
        const std::string whitespace = " \t\n\r\f\v";
        s.erase(s.find_last_not_of(whitespace) + 1);
        s.erase(0, s.find_first_not_of(whitespace));
        while (!s.empty() && s.back() == '\0') {
            s.pop_back();
        }
    }
    void cleanFieldsInfo() {
        for (int i = 0; i < fieldsInfo.size(); i++) {
            for (int j = 0; j < fieldsInfo[i].size(); j++) {
                myTrim(fieldsInfo[i][j]);
            }
        }
    }
    void parseInsert(std::string query) {
        typeQuery = "INSERT";

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
            tokens1.push_back(value);
        }
    }
    void checkType() {
        for (int i = 0; i < tokens2.size(); i++) {
            if (tokens2[i] == "WHERE") {
                fieldToTree = tokens2[i + 1];
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
    void specifyFields(std::vector<std::vector<std::string>> inRegs) {
        std::vector<int> activeIndexes;
        for (int i = 0; i < dataInfo.size();i++) {
            for (int j = 0; j < tokens1.size(); j++) {
                if (dataInfo[i][0] == tokens1[j]) {
                    finalFields.push_back(tokens1[j]);
                    activeIndexes.push_back(i);
                    break;
                }
            }
        }
        for (int i = 0; i < inRegs.size(); i++) {
            std::vector<std::string> Reg;
            for (int j = 0; j < inRegs[i].size(); j++) {
                for (int k = 0; k < activeIndexes.size(); k++) {
                    if (activeIndexes[k] == j) {
                        Reg.push_back(inRegs[i][j]);
                        break;
                    }
                }
            }
            finalRegs.push_back(Reg);
        }
    }
};