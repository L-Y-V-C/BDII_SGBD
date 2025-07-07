#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

#include "listRegister.hh"
#include "register.hh"
#include "interface.hh"

#include "dataReader.hh"
#include "queryManager.hh"
#include "diskIterator.hh"

#include "functions.hh"

int main()
{
    Disk disk(4, 4, 8, 20);
    DiskManager diskManager(disk);
    DataReader dataReader;

    std::string data_path("data_test.csv"),
                table_data_path("table_data.csv"),
                disk_path("disk.txt"),
                meta_data_path("meta_data.txt");
   

    std::string data_str = dataReader.read_data(data_path, table_data_path);
    
    std::cout << "Tabla: " << dataReader.table_name << "\n\n";
    std::cout << data_str << "\n\n";

    dataReader.write_data(diskManager, data_str, meta_data_path);
    dataReader.write_data_on_txt(
                                 disk_path,
                                 data_str,
                                 disk);
    //disk.print_disk();


    std::vector<int> id_to_find{ 1, 3, 4, 11 };


    std::vector<std::vector<std::string>> meta_data_info;
    meta_data_info = dataReader.read_meta_data(meta_data_path, id_to_find);

    std::cout << "Registros buscados:\n";
    for (auto i : id_to_find)
        std::cout << i << " ";
    std::cout << "\n\n\n";

    std::cout << "Meta data encontrada:\n";
    for (auto i : meta_data_info)
    {
        for (auto j : i)
            std::cout << j << " ";
        std::cout << "\n\n";
    }
    std::cout << "\n";

   
    // TEST PRUEBA EXTRACION DE REGISTROS DE METADATA
    DiskIterator disk_iterator(disk, dataReader.get_register_size());  // , dataReader.get_field_size());

    std::vector<std::string> answer_query;
    answer_query = disk_iterator.iterateAndExtractRegs(meta_data_info);

    std::vector<std::vector<std::string>> query_separated;
    for (int i = 0; i < answer_query.size(); i++)
        query_separated.push_back(separate_string(answer_query[i], ','));


    std::cout << "Registros encontrados:\n";
    print_table(query_separated, 50);

    printf("\n");
    
    //dataReader.debug();
}
