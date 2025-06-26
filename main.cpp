#include <iostream>
#include <string>

#include "dataReader.hh"

int main()
{
    Disk test_disk(2, 2, 30, 10);
    SectorIterator test_iterator(test_disk);
    DataReader test;


    std::string data_path("data_test.csv"),
                table_data_path("table_data.csv"),
                disk_path("disk.txt"),
                meta_data_path("meta_data.txt");

    std::string data_str = test.read_data(data_path, table_data_path);


    test.debug();
    std::cout << data_str << "\n\n\n\n\n\n\n";

    test.write_data(test_iterator, data_str, meta_data_path);
    test_disk.print_disk();

    test.write_data_on_disk(disk_path,
                            data_str,
                            test_disk);

}