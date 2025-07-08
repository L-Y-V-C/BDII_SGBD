#pragma once

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

#include "tree.hh"

int main()
{
    Disk disk(4, 4, 8, 20);
    DiskManager diskManager(disk);
    DataReader dataReader;

    std::string data_path("taxables.csv"),
                table_data_path("struct_table.txt"),
                disk_path("disk.txt"),
                meta_data_path("meta_data.txt");
   

    std::string data_str = dataReader.read_data(data_path, table_data_path);
    
    std::cout << "Tabla: " << dataReader.table_name << "\n\n";

    for (auto c : data_str)
    {
        if (c == '\0')
            std::cout << "-";
        else
            std::cout << c;
    }
    std::cout << "\n";

    dataReader.write_data(diskManager, data_str, meta_data_path);
    dataReader.write_data_on_txt(
                                 disk_path,
                                 data_str,
                                 disk);
    //disk.print_disk();

    

    std::vector<int> id_to_find{ 1, 3, 4, 11 };

    
    std::vector<std::vector<std::string>> meta_data_info;

    meta_data_info = dataReader.read_all_meta_data(meta_data_path);
    //meta_data_info = dataReader.read_meta_data(meta_data_path, id_to_find);

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

    std::vector<std::vector<std::string>> answer_query = disk_iterator.iterateAndExtractRegs(meta_data_info);

    std::cout << "Registros encontrados:\n";
    print_table(answer_query, 50);

    printf("\n--------\n");
    //dataReader.debug();
    QueryManager qm;
    qm.dataInfo = dataReader.data_info;
    //std::string q5 = "SELECT item, id FROM PRODUCTO WHERE item = \"Fruit of the Loom Girl's Socks\"";
    std::string q5 = "SELECT item, id FROM PRODUCTO WHERE cost = 10.00";
    //std::string q5 = "SELECT name, id FROM tabla";
    //std::string q5 = "INSERT INTO PRODUCTO VALUES(12, \"Fruit of the Loom Girl's Socks\", 123.54)";
    qm.parseQuery(q5);
    qm.printTokens();
    printf("\n--------\n");
    //dataReader.debug();
}
