#include <iostream>
#include <string>

#include "dataReader.hh"

int main()
{
    Disk test_disk(2, 3, 3, 10);
    SectorIterator test_iterator(test_disk);
    DataReader test;

    

    std::string data_str = test.read_data("C:\\Users\\diego\\OneDrive\\Escritorio\\bd_data\\data_test.csv",
        "C:\\Users\\diego\\OneDrive\\Escritorio\\bd_data\\table_data.csv");

    test.debug();

    
    std::cout << data_str << "\n\n\n\n\n\n\n";

    test.write_data(test_iterator, data_str);
    test_disk.print_disk();

}