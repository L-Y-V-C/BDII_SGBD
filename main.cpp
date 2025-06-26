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

int main()
{
    Disk test_disk(4, 4, 8, 20);
    DiskManager testDiskManager(test_disk);
    DataReader testDataReader;

    std::string data_path("data_test.csv"),
                table_data_path("table_data.csv"),
                disk_path("disk.txt"),
                meta_data_path("meta_data.txt");
    
    std::string data_str = testDataReader.read_data(data_path, table_data_path);

    testDataReader.debug();

    std::cout << data_str << "\n\n\n\n\n\n\n";

    testDataReader.write_data(testDiskManager, data_str, meta_data_path);
    test_disk.print_disk();

    testDataReader.write_data_on_disk(disk_path,
                            data_str,
                            test_disk);
    
    std::vector<int> ids{ 1, 3, 4, 11 };
    std::vector<std::vector<std::string>> t1;
    t1 = testDataReader.read_meta_data(meta_data_path, ids);
    /*
    printf("\ntest read meta data\n");
    for (auto i : t1) {
        for (auto j : i)
            std::cout << j << " ";
        printf("\n");
    }
    */
    //std::cout << "\n\n" << data_str << "\n\n";

    std::vector<size_t> test_pos;
    /*
    test_pos = testDiskManager.get_position();
    for (auto i : test_pos)
        printf("%d ", i);
    std::cout << "\n\n" << testDiskManager.disk.plates[0].surfaces[0].tracks[0].sectors[0].data << "\n\n";
    */

    /* TEST PRUEBA EXTRACION DE REGISTROS DE METADATA*/
    DiskIterator tDM2(test_disk);
    
    tDM2.fieldSizes = testDataReader.extractSizes();
    tDM2.calculateRegSize();

    std::vector<std::string> testRpta;
    testRpta = tDM2.iterateAndExtractRegs(t1);
    for (auto i : testRpta)
        std::cout << i << "\n";
    //printf("\n%d\n", testDiskManager.regSize);
    /*
    char ct;
    printf("\nin main\nstr: ");
    for (int i = 0; i < tDM2.regSize; i++) {
        ct = tDM2.current_char();
        std::cout <<"" << ct;
        tDM2.next();
    }
    */
    printf("\n");
    /*
    test_pos = tDM2.get_position();
    for (auto i : test_pos)
        printf("%d ", i);
    */
    //std::cout << "\n\n" << tDM2.disk.plates[0].surfaces[0].tracks[0].sectors[0].data[0] << "\n\n";
    /*
    printf("\n\n\n\n");
    std::vector<std::vector<std::string>> str2test = test.data_info;
    for (auto i : str2test) {
        std::cout << i[2] << "\n";
    }
    */
}
