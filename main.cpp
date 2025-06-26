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


int main() {
    std::vector<std::string> columnas = { "Index", "Item", "Cost", "Tax", "Total" };

    ListRegister tabla(columnas);

    tabla.add({ "1", "Apple", "1.20", "0.10", "1.30" });
    tabla.add({ "2", "Banana", "0.80", "0.05", "0.85" });
    tabla.add({ "3", "Orange", "1.00", "0.08", "1.08" });

    // Imprimir
    tabla.print();
}


/*
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    App app;
    app.Run(hInstance, nCmdShow);
    return 0;
}
*/