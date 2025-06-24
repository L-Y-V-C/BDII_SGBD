#include <iostream>
#include <string>

#include "dataReader.hh"

int main()
{
    //DataReader test;
    //std::cout << test.read_data("C:\\Users\\diego\\OneDrive\\Escritorio\\data_test.csv",
    //                            "C:\\Users\\diego\\OneDrive\\Escritorio\\meta_data.csv") << "\n";

    std::string file1 = "struct_table.txt";
    DataReader dr;
    dr.archiveDataStruct = file1;
    dr.resizeDataStruct();
    dr.fillDataStructFromArchive();
    int n = dr.countLinesOfTheArchive();
    std::cout << dr.ds.tableName << "\n";
    for (int i = 0; i < n; i++)
        std::cout << dr.ds.field[i] << " " << dr.ds.number[i] << "\n";
}