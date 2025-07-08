#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdio.h>

#include "listRegister.hh"
#include "register.hh"
#include "interface.hh"
#include "dataReader.hh"
#include "queryManager.hh"
#include "diskIterator.hh"
#include "functions.hh"
#include "tree.hh"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Simulados, remplaza con tus datos reales:
std::vector<std::string> finalFields;
std::vector<std::vector<std::string>> finalRegs;

std::vector<std::vector<std::string>> meta_data;

std::string charArrayToString(const char* charArray) {
    return std::string(charArray);
}

void reset_all(Disk& in_disk, DiskManager& in_manager, DataReader& in_reader)
{
    in_disk.clear();
    in_manager.reset();
    in_reader.reset();
}

int main()
{
    // Inicializar GLFW
    if (!glfwInit()) return -1;
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW/GLAD", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    // Inicializar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Variables de interfaz
    int numberInputs[4] = { 0 };
    char fileNames[2][128] = {};
    char queryInput[256] = {};

    std::string data_path,
        table_data_path,
        disk_path("C:\\Users\\tanuki\\source\\repos\\SGDB_DBII_UI\\source\\disk.txt"),
        meta_data_path("C:\\Users\\tanuki\\source\\repos\\SGDB_DBII_UI\\source\\meta_data.txt");
    std::string query;

    bool showTable = false;
    bool showMetaDataPopup = false;
    int metaDataIndex = -1;

    // Variables
    Disk disk;
    DiskManager diskManager(disk);
    DataReader dataReader;

    std::vector<std::vector<std::string>> allData;
    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Simulador BDII SGBD");

        ImGui::Separator();
        ImGui::Text("Entradas Numericas:");
        const char* labels[4] = { "Num Platos", "Pistas", "Sectores", "Tamano Sector" };
        for (int i = 0; i < 4; i++)
        {
            ImGui::SetNextItemWidth(100);
            ImGui::InputScalar(labels[i], ImGuiDataType_S32, &numberInputs[i], NULL, NULL, "%d", ImGuiInputTextFlags_CharsDecimal);
        }

        ImGui::Separator();
        ImGui::InputText("data", fileNames[0], IM_ARRAYSIZE(fileNames[0]));
        ImGui::InputText("struct", fileNames[1], IM_ARRAYSIZE(fileNames[1]));

        if (ImGui::Button("Inicializar"))
        {
            printf("Inicializar presionado\n");
            // Aqu� enlaza Disk, DataReader y l�gica
            
            //reset_all(disk, diskManager, dataReader);
            disk.assign_size(6, 8, 10, 10);
            //disk.assign_size(numberInputs[0], numberInputs[1], numberInputs[2], numberInputs[3]);
            //data_path = charArrayToString(fileNames[0]);
            //table_data_path = charArrayToString(fileNames[1]);
            data_path = "C:\\Users\\tanuki\\source\\repos\\SGDB_DBII_UI\\source\\taxables.csv";
            table_data_path = "C:\\Users\\tanuki\\source\\repos\\SGDB_DBII_UI\\source\\struct_table.txt";

            std::string data_str = dataReader.read_data(data_path, table_data_path);
            dataReader.write_data(diskManager, data_str, meta_data_path);
            dataReader.debug();
            dataReader.write_data_on_txt(
                disk_path,
                data_str,
                disk);
            //std::vector<int> firstIdsToFill;
            //for (int i = 1; i <= dataReader.register_count; i++) {
            //    firstIdsToFill.push_back(i);
            //}
            std::vector<std::vector<std::string>> meta_data_info;
            meta_data_info = dataReader.read_all_meta_data(meta_data_path);
            DiskIterator disk_iterator(disk, dataReader.get_register_size());
            allData = disk_iterator.iterateAndExtractRegs(meta_data_info);
            //for(auto i : dataReader)
            //finalFields = dataReader.data_info[0];
            //finalRegs = allData;
            //showTable = true;
        }

        ImGui::Separator();
        ImGui::InputText("Consulta", queryInput, IM_ARRAYSIZE(queryInput));
        if (ImGui::Button("GO"))
        {
            printf("GO presionado: %s\n", queryInput);
            // Aqu� ejecuta consulta y llena finalFields y finalRegs
            query = charArrayToString(queryInput);
            QueryManager qm;
            qm.dataInfo = dataReader.data_info;
            qm.fieldsInfo = allData;
            qm.parseQuery(query);
            if (qm.typeQuery == "SELECT") {
                std::vector<std::vector<std::string>>meta_data_info = dataReader.read_meta_data(meta_data_path, qm.idsQueryResult);
                meta_data = meta_data_info;
                DiskIterator disk_iterator(disk, dataReader.get_register_size());
                std::vector<std::vector<std::string>> answer_query = disk_iterator.iterateAndExtractRegs(meta_data_info);
                qm.specifyFields(answer_query);
                finalFields = qm.finalFields;
                finalRegs = qm.finalRegs;
            }
            else if (qm.typeQuery == "INSERT") {
                dataReader.insert_query(diskManager, qm.tokens1, meta_data_path, data_path);
            }

            showTable = true;
        }

        if (showTable)
        {
            ImGui::Separator();
            ImGui::Text("Resultados de Consulta:");
            if (ImGui::BeginTable("Table", finalFields.size() + 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                for (const auto& field : finalFields)
                    ImGui::TableSetupColumn(field.c_str());
                ImGui::TableSetupColumn("Ver Lugar");
                ImGui::TableHeadersRow();

                for (int row = 0; row < finalRegs.size(); ++row)
                {
                    ImGui::TableNextRow();
                    for (int col = 0; col < finalFields.size(); ++col)
                    {
                        ImGui::TableSetColumnIndex(col);
                        ImGui::Text("%s", finalRegs[row][col].c_str());
                    }
                    ImGui::TableSetColumnIndex(finalFields.size());
                    if (ImGui::Button(("Ver##" + std::to_string(row)).c_str()))
                    {
                        showMetaDataPopup = true;
                        metaDataIndex = row;
                    }
                }
                ImGui::EndTable();
            }
        }

        if (showMetaDataPopup && metaDataIndex != -1)
            ImGui::OpenPopup("Ver Metadata");

        if (ImGui::BeginPopupModal("Ver Metadata", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Ubicacion del registro %d:", metaDataIndex);
            for (int i = 1; i < meta_data[metaDataIndex].size(); i++)
            {
                if (i > 0 && i % 5 == 0)
                    ImGui::Separator();
                ImGui::Text("%s", meta_data[metaDataIndex][i].c_str());
            }
            if (ImGui::Button("Cerrar"))
            {
                ImGui::CloseCurrentPopup();
                showMetaDataPopup = false;
                metaDataIndex = -1;
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}