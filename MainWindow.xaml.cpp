#pragma once

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Input;
using namespace Microsoft::UI::Xaml::Media;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI;

//Disk disk(4, 4, 4, 4);

namespace winrt::DiscoSimulador::implementation
{
    MainWindow::MainWindow() : configuracion("", "", "", "", "")
    {
        InitializeComponent();
        Title(L"Simulador de Disco Duro");
        LimpiarResultados();
    }

    void MainWindow::InicializarButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        try
        {
            // Obtener valores de configuración del disco
            numPlatos = ObtenerValorComboBox(PlatosComboBox());
            numPistas = ObtenerValorComboBox(PistasComboBox());
            numSectores = ObtenerValorComboBox(SectoresComboBox());
            tamanoSector = ObtenerValorComboBox(TamanoSectorComboBox());

            // Obtener nombres de archivos de los TextBox
            std::wstring estructuraW = ArchivoEstructuraTextBox().Text().c_str();
            std::wstring csvW = ArchivoCSVTextBox().Text().c_str();

            nombreArchivoEstructura = WStringToString(estructuraW);
            nombreArchivoCSV = WStringToString(csvW);

            // Validar que no estén vacíos
            if (nombreArchivoEstructura.empty())
                nombreArchivoEstructura = "struct_table.txt";
            if (nombreArchivoCSV.empty())
                nombreArchivoCSV = "taxables.csv";

            // Crear configuración (ya no necesitamos superficies, solo platos y pistas)
            configuracion = ConfiguracionDisco(numPlatos, "2", numPistas, numSectores, tamanoSector);

            // Resetear posición del disco
            currentPlate = 0;
            currentSurface = 0;
            currentTrack = 0;
            currentSector = 0;
            currentChar = 0;

            // Cargar archivos
            if (!CargarEstructuraTabla())
            {
                MostrarError("Error al cargar struct_table.txt");
                return;
            }

            if (!CargarDatosCSV())
            {
                MostrarError("Error al cargar taxables.csv");
                return;
            }

            // Marcar como inicializado
            discoInicializado = true;

            // Mostrar información
            MostrarDatos();

            // Mostrar panel de consulta
            ConsultaPanel().Visibility(Visibility::Visible);
            InstruccionesPanel().Visibility(Visibility::Collapsed);

            // Enfocar TextBox de consulta
            ConsultaTextBox().Focus(FocusState::Programmatic);
        }
        catch (...)
        {
            MostrarError("Error al inicializar el disco duro.");
        }
    }

    void MainWindow::EjecutarButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        EjecutarConsulta();
    }

    void MainWindow::ConsultaTextBox_KeyDown(IInspectable const&, KeyRoutedEventArgs const& e)
    {
        if (e.Key() == VirtualKey::Enter)
        {
            EjecutarConsulta();
        }
    }

    void MainWindow::ToggleMetadataButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Button>();

        // El botón está dentro de un Grid, que está dentro de un Border, que está dentro de un StackPanel
        // Necesitamos navegar correctamente por la jerarquía de controles

        // 1. Obtener el Grid padre del botón
        auto dataGrid = button.Parent().as<Grid>();
        if (!dataGrid) return;

        // 2. Obtener el Border que contiene el Grid
        auto dataBorder = dataGrid.Parent().as<Border>();
        if (!dataBorder) return;

        // 3. Obtener el StackPanel que contiene tanto el Border de datos como el Border de metadata
        auto mainPanel = dataBorder.Parent().as<StackPanel>();
        if (!mainPanel) return;

        // 4. El panel de metadata debe ser el segundo hijo del StackPanel
        if (mainPanel.Children().Size() >= 2)
        {
            auto metadataBorder = mainPanel.Children().GetAt(1).as<Border>();
            if (metadataBorder)
            {
                // Toggle visibility
                if (metadataBorder.Visibility() == Visibility::Visible)
                {
                    metadataBorder.Visibility(Visibility::Collapsed);
                    button.Content(box_value(L"📍 Mostrar Ubicación"));
                }
                else
                {
                    metadataBorder.Visibility(Visibility::Visible);
                    button.Content(box_value(L"📍 Ocultar Ubicación"));
                }
            }
        }
    }

    void MainWindow::EjecutarConsulta()
    {
        if (!discoInicializado)
        {
            MostrarError("Primero debe inicializar el disco duro.");
            return;
        }

        std::wstring consultaW = ConsultaTextBox().Text().c_str();
        std::string consulta = WStringToString(consultaW);

        // Eliminar espacios en blanco
        consulta.erase(0, consulta.find_first_not_of(" \t\n\r"));
        consulta.erase(consulta.find_last_not_of(" \t\n\r") + 1);

        if (consulta.empty())
        {
            MostrarError("Por favor ingrese una consulta.");
            return;
        }

        try
        {
            // Procesar consulta
            TablaDatos resultadoTabla = ProcesarConsulta(consulta);

            // Mostrar consulta ejecutada
            MostrarConsultaEjecutada(consulta);

            // Mostrar resultados
            MostrarTablaResultados(resultadoTabla);

            // Limpiar TextBox
            ConsultaTextBox().Text(L"");
        }
        catch (...)
        {
            MostrarError("Error al ejecutar la consulta.");
        }
    }

    void MainWindow::MostrarConfiguracion()
    {
        std::string textoConfig = "Número de Platos: " + configuracion.numPlatos + "\n";
        textoConfig += "Superficies por Plato: " + configuracion.numSuperficies + "\n";
        textoConfig += "Pistas por Superficie: " + configuracion.numPistas;
    }

    void MainWindow::MostrarEstructura()
    {
        // Implementación si es necesaria
    }

    void MainWindow::MostrarDatos()
    {
        // Limpiar contenedor anterior
        DatosContainer().Children().Clear();

        // Crear encabezado
        if (!camposCSV.empty())
        {
            auto encabezado = CrearFilaTabla(camposCSV, true);
            DatosContainer().Children().Append(encabezado);
        }

        // Crear filas de datos
        for (const auto& fila : datosCSV)
        {
            auto filaUI = CrearFilaTabla(fila, false);
            DatosContainer().Children().Append(filaUI);
        }

        DatosPanel().Visibility(Visibility::Visible);
    }

    void MainWindow::MostrarConsultaEjecutada(const std::string& consulta)
    {
        ConsultaEjecutadaTextBlock().Text(StringToWString(consulta));
        ConsultaEjecutadaPanel().Visibility(Visibility::Visible);
    }

    void MainWindow::MostrarTablaResultados(const TablaDatos& tabla)
    {
        // Limpiar contenedor anterior
        ResultadosContainer().Children().Clear();

        // Crear encabezado con columna adicional para el botón
        if (!tabla.fields.empty())
        {
            auto fieldsWithButton = tabla.fields;
            fieldsWithButton.push_back("Ubicación Física");
            auto encabezado = CrearFilaTabla(fieldsWithButton, true);
            ResultadosContainer().Children().Append(encabezado);
        }

        // Crear filas de datos con metadata
        for (const auto& fila : tabla.data)
        {
            auto filaUI = CrearFilaTablaConMetadata(fila, false);
            ResultadosContainer().Children().Append(filaUI);
        }

        TablaPanel().Visibility(Visibility::Visible);
    }

    void MainWindow::LimpiarResultados()
    {
        ConsultaEjecutadaPanel().Visibility(Visibility::Collapsed);
        TablaPanel().Visibility(Visibility::Collapsed);
    }

    void MainWindow::MostrarError(const std::string& mensaje)
    {
        ContentDialog dialog;
        dialog.Title(box_value(L"Error"));
        dialog.Content(box_value(StringToWString(mensaje)));
        dialog.CloseButtonText(L"OK");
        dialog.XamlRoot(this->Content().XamlRoot());
        dialog.ShowAsync();
    }

    bool MainWindow::CargarEstructuraTabla()
    {
        try
        {
            std::ifstream archivo(nombreArchivoEstructura);
            if (!archivo.is_open())
            {
                // Si no existe el archivo, usar estructura por defecto
                estructuraTabla = "CREATE TABLE PRODUCTO(\n";
                estructuraTabla += "    index INTEGER(10) PRIMARY KEY,\n";
                estructuraTabla += "    item VARCHAR(40) NOT NULL,\n";
                estructuraTabla += "    cost DECIMAL(10, 2) NOT NULL,\n";
                estructuraTabla += "    tax DECIMAL(10, 2) NOT NULL,\n";
                estructuraTabla += "    total DECIMAL(10, 2) NOT NULL\n";
                estructuraTabla += ");";
                return true;
            }

            std::string linea;
            estructuraTabla = "";
            while (std::getline(archivo, linea))
            {
                estructuraTabla += linea + "\n";
            }
            archivo.close();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool MainWindow::CargarDatosCSV()
    {
        try
        {
            std::ifstream archivo(nombreArchivoCSV);
            if (!archivo.is_open())
            {
                // Si no existe el archivo, usar datos por defecto
                camposCSV = { "Index", "Item", "Cost", "Tax", "Total" };

                datosCSV = {
                    {"1", "Fruit of the Loom Girl's Socks", "7.97", "0.60", "8.57"},
                    {"2", "Rawlings Little League Baseball", "2.97", "0.22", "3.19"},
                    {"3", "Secret Antiperspirant", "1.29", "0.10", "1.39"},
                    {"4", "Deadpool DVD", "14.96", "1.12", "16.08"},
                    {"5", "Maxwell House Coffee 28 oz", "7.28", "0.55", "7.83"},
                    {"6", "Banana Boat Sunscreen, 8 oz", "6.68", "0.50", "7.18"},
                    {"7", "Wrench Set, 18 pieces", "10.00", "0.75", "10.75"},
                    {"8", "M and M, 42 oz", "8.98", "0.67", "9.65"},
                    {"9", "Bertoli Alfredo Sauce", "2.12", "0.16", "2.28"},
                    {"10", "Large Paperclips, 10 boxes", "6.19", "0.46", "6.65"}
                };
                return true;
            }

            std::string linea;
            bool primeraLinea = true;

            while (std::getline(archivo, linea))
            {
                auto campos = SplitString(linea, ',');

                if (primeraLinea)
                {
                    camposCSV = campos;
                    primeraLinea = false;
                }
                else
                {
                    datosCSV.push_back(campos);
                }
            }
            archivo.close();
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    // MÉTODOS DE METADATA Y UBICACIONES FÍSICAS

    UbicacionFisica MainWindow::CalcularUbicacionInicio(const std::string& valor)
    {
        return UbicacionFisica(currentPlate, currentSurface, currentTrack, currentSector, currentChar);
    }

    UbicacionFisica MainWindow::CalcularUbicacionFin(const UbicacionFisica& inicio, const std::string& valor)
    {
        int longitudTotal = valor.length();

        // Simular avance en el disco
        int tempPlate = inicio.plateIndex;
        int tempSurface = inicio.surfaceIndex;
        int tempTrack = inicio.trackIndex;
        int tempSector = inicio.sectorIndex;
        int tempChar = inicio.charIndex + longitudTotal - 1;

        // Configuración del disco
        int maxPlatos = std::stoi(configuracion.numPlatos);
        int maxSuperficies = std::stoi(configuracion.numSuperficies);
        int maxPistas = std::stoi(configuracion.numPistas);
        int maxSectores = std::stoi(numSectores);
        int maxChars = std::stoi(tamanoSector);

        // Manejar overflow de caracteres
        while (tempChar >= maxChars)
        {
            tempChar -= maxChars;
            tempSector++;

            if (tempSector >= maxSectores)
            {
                tempSector = 0;
                tempTrack++;

                if (tempTrack >= maxPistas)
                {
                    tempTrack = 0;
                    tempSurface++;

                    if (tempSurface >= maxSuperficies)
                    {
                        tempSurface = 0;
                        tempPlate++;

                        if (tempPlate >= maxPlatos)
                        {
                            tempPlate = 0; // Wrap around
                        }
                    }
                }
            }
        }

        return UbicacionFisica(tempPlate, tempSurface, tempTrack, tempSector, tempChar);
    }

    void MainWindow::AvanzarPosicionDisco(int caracteres)
    {
        int maxPlatos = std::stoi(configuracion.numPlatos);
        int maxSuperficies = std::stoi(configuracion.numSuperficies);
        int maxPistas = std::stoi(configuracion.numPistas);
        int maxSectores = std::stoi(numSectores);
        int maxChars = std::stoi(tamanoSector);

        currentChar += caracteres;

        while (currentChar >= maxChars)
        {
            currentChar -= maxChars;
            currentSector++;

            if (currentSector >= maxSectores)
            {
                currentSector = 0;
                currentTrack++;

                if (currentTrack >= maxPistas)
                {
                    currentTrack = 0;
                    currentSurface++;

                    if (currentSurface >= maxSuperficies)
                    {
                        currentSurface = 0;
                        currentPlate++;

                        if (currentPlate >= maxPlatos)
                        {
                            currentPlate = 0;
                        }
                    }
                }
            }
        }
    }

    FilaConMetadata MainWindow::CrearFilaConMetadata(const std::vector<std::string>& fila)
    {
        std::vector<MetadataCampo> metadata;

        for (size_t i = 0; i < fila.size(); i++)
        {
            const std::string& valor = fila[i];
            const std::string& nombreCampo = (i < camposCSV.size()) ? camposCSV[i] : "Campo" + std::to_string(i);

            UbicacionFisica inicio = CalcularUbicacionInicio(valor);
            UbicacionFisica fin = CalcularUbicacionFin(inicio, valor);

            metadata.emplace_back(nombreCampo, valor, inicio, fin);

            // Avanzar posición para el próximo campo
            AvanzarPosicionDisco(valor.length() + 1); // +1 para el separador
        }

        return FilaConMetadata(fila, metadata);
    }

    TablaDatos MainWindow::ProcesarConsulta(const std::string& consulta)
    {
        // Buscar coincidencias en los datos
        std::vector<FilaConMetadata> coincidencias = BuscarCoincidencias(consulta);

        // Crear tabla resultado
        return TablaDatos(camposCSV, coincidencias);
    }

    std::vector<FilaConMetadata> MainWindow::BuscarCoincidencias(const std::string& consulta)
    {
        std::vector<FilaConMetadata> coincidencias;

        // Resetear posición del disco
        currentPlate = 0;
        currentSurface = 0;
        currentTrack = 0;
        currentSector = 0;
        currentChar = 0;

        // Convertir consulta a minúsculas para búsqueda insensible a mayúsculas
        std::string consultaLower = consulta;
        std::transform(consultaLower.begin(), consultaLower.end(), consultaLower.begin(), ::tolower);

        // Buscar en todos los datos
        for (const auto& fila : datosCSV)
        {
            bool encontrado = false;

            // Buscar en cada campo de la fila
            for (const auto& campo : fila)
            {
                std::string campoLower = campo;
                std::transform(campoLower.begin(), campoLower.end(), campoLower.begin(), ::tolower);

                if (campoLower.find(consultaLower) != std::string::npos)
                {
                    encontrado = true;
                    break;
                }
            }

            if (encontrado)
            {
                FilaConMetadata filaConMeta = CrearFilaConMetadata(fila);
                coincidencias.push_back(filaConMeta);
            }
            else
            {
                // Simular que procesamos esta fila aunque no la incluyamos
                for (const auto& campo : fila)
                {
                    AvanzarPosicionDisco(campo.length() + 1);
                }
            }
        }

        return coincidencias;
    }

    // MÉTODOS DE CREACIÓN DE UI

    Microsoft::UI::Xaml::Controls::Border MainWindow::CrearFilaTabla(const std::vector<std::string>& fila, bool esEncabezado)
    {
        Border border;
        border.Height(35);
        border.BorderThickness(ThicknessHelper::FromUniformLength(1));
        border.BorderBrush(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) }); // #A6856A

        if (esEncabezado)
        {
            border.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) }); // #A6856A
        }
        else
        {
            border.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 231, 227, 224) }); // #E7E3E0
        }

        Grid grid;

        // Crear columnas dinámicamente
        for (size_t i = 0; i < fila.size(); i++)
        {
            auto columnDef = ColumnDefinition();
            columnDef.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
            grid.ColumnDefinitions().Append(columnDef);
        }

        // Crear TextBlocks para cada campo
        for (size_t i = 0; i < fila.size(); i++)
        {
            TextBlock textBlock;
            textBlock.Text(StringToWString(fila[i]));
            textBlock.VerticalAlignment(VerticalAlignment::Center);
            textBlock.HorizontalAlignment(HorizontalAlignment::Center);
            textBlock.FontSize(12);
            textBlock.Margin(ThicknessHelper::FromUniformLength(5));
            textBlock.TextWrapping(TextWrapping::Wrap);

            if (esEncabezado)
            {
                textBlock.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
                textBlock.Foreground(SolidColorBrush{ Colors::White() });
            }
            else
            {
                textBlock.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 155, 135, 132) }); // #9B8784
            }

            Grid::SetColumn(textBlock, static_cast<int>(i));
            grid.Children().Append(textBlock);
        }

        border.Child(grid);
        return border;
    }

    Microsoft::UI::Xaml::Controls::Border MainWindow::CrearFilaTablaConMetadata(const FilaConMetadata& fila, bool esEncabezado)
    {
        Border mainBorder;
        mainBorder.BorderThickness(ThicknessHelper::FromUniformLength(1));
        mainBorder.BorderBrush(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        mainBorder.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 231, 227, 224) });

        StackPanel mainPanel;
        mainPanel.Orientation(Orientation::Vertical);

        // Crear fila de datos con botón
        Border dataBorder;
        dataBorder.Height(35);
        dataBorder.BorderThickness(ThicknessHelper::FromUniformLength(1));
        dataBorder.BorderBrush(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        dataBorder.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 231, 227, 224) });

        Grid dataGrid;

        // Crear columnas dinámicamente (datos + botón)
        for (size_t i = 0; i < fila.datos.size(); i++)
        {
            auto columnDef = ColumnDefinition();
            columnDef.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
            dataGrid.ColumnDefinitions().Append(columnDef);
        }

        // Columna para el botón
        auto buttonColumnDef = ColumnDefinition();
        buttonColumnDef.Width(GridLengthHelper::FromValueAndType(150, GridUnitType::Pixel));
        dataGrid.ColumnDefinitions().Append(buttonColumnDef);

        // Crear TextBlocks para cada campo
        for (size_t i = 0; i < fila.datos.size(); i++)
        {
            TextBlock textBlock;
            textBlock.Text(StringToWString(fila.datos[i]));
            textBlock.VerticalAlignment(VerticalAlignment::Center);
            textBlock.HorizontalAlignment(HorizontalAlignment::Center);
            textBlock.FontSize(12);
            textBlock.Margin(ThicknessHelper::FromUniformLength(5));
            textBlock.TextWrapping(TextWrapping::Wrap);
            textBlock.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 155, 135, 132) });

            Grid::SetColumn(textBlock, static_cast<int>(i));
            dataGrid.Children().Append(textBlock);
        }

        // Crear botón para toggle metadata
        Button toggleButton;
        toggleButton.Content(box_value(L"📍 Mostrar Ubicación"));
        toggleButton.FontSize(10);
        toggleButton.Padding(ThicknessHelper::FromLengths(5, 2, 5, 2));
        toggleButton.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        toggleButton.Foreground(SolidColorBrush{ Colors::White() });
        toggleButton.BorderBrush(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        toggleButton.VerticalAlignment(VerticalAlignment::Center);
        toggleButton.HorizontalAlignment(HorizontalAlignment::Center);
        toggleButton.Click({ this, &MainWindow::ToggleMetadataButton_Click });

        Grid::SetColumn(toggleButton, static_cast<int>(fila.datos.size()));
        dataGrid.Children().Append(toggleButton);

        dataBorder.Child(dataGrid);
        mainPanel.Children().Append(dataBorder);

        // Crear panel de metadata (inicialmente oculto)
        Border metadataBorder;
        metadataBorder.Background(SolidColorBrush{ ColorHelper::FromArgb(255, 245, 241, 238) });
        metadataBorder.BorderThickness(ThicknessHelper::FromUniformLength(1));
        metadataBorder.BorderBrush(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        metadataBorder.Padding(ThicknessHelper::FromUniformLength(10));
        metadataBorder.Visibility(Visibility::Collapsed);

        StackPanel metadataPanel;
        metadataPanel.Orientation(Orientation::Vertical);

        TextBlock metadataTitle;
        metadataTitle.Text(L"Ubicación Física en Disco:");
        metadataTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        metadataTitle.FontSize(11);
        metadataTitle.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 166, 133, 106) });
        metadataTitle.Margin(ThicknessHelper::FromLengths(0, 0, 0, 5));
        metadataPanel.Children().Append(metadataTitle);

        // Crear información de metadata para cada campo
        for (const auto& metadata : fila.metadata)
        {
            auto metadataFieldPanel = CrearPanelMetadata(metadata);
            metadataPanel.Children().Append(metadataFieldPanel);
        }

        metadataBorder.Child(metadataPanel);
        mainPanel.Children().Append(metadataBorder);

        mainBorder.Child(mainPanel);
        return mainBorder;
    }

    Microsoft::UI::Xaml::Controls::StackPanel MainWindow::CrearPanelMetadata(const MetadataCampo& metadata)
    {
        StackPanel panel;
        panel.Orientation(Orientation::Horizontal);
        panel.Margin(ThicknessHelper::FromLengths(0, 2, 0, 2));

        // Nombre del campo
        TextBlock fieldName;
        fieldName.Text(StringToWString(metadata.nombreCampo + ":"));
        fieldName.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        fieldName.FontSize(10);
        fieldName.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 155, 135, 132) });
        fieldName.Width(100);
        panel.Children().Append(fieldName);

        // Ubicación de inicio
        TextBlock startLocation;
        startLocation.Text(StringToWString("Inicio: " + metadata.inicio.ToString()));
        startLocation.FontSize(10);
        startLocation.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 155, 135, 132) });
        startLocation.Margin(ThicknessHelper::FromLengths(10, 0, 0, 0));
        panel.Children().Append(startLocation);

        // Ubicación de fin
        TextBlock endLocation;
        endLocation.Text(StringToWString(" | Fin: " + metadata.fin.ToString()));
        endLocation.FontSize(10);
        endLocation.Foreground(SolidColorBrush{ ColorHelper::FromArgb(255, 155, 135, 132) });
        panel.Children().Append(endLocation);

        return panel;
    }

    // MÉTODOS DE UTILIDAD

    std::string MainWindow::ObtenerValorComboBox(ComboBox const& comboBox)
    {
        auto selectedItem = comboBox.SelectedItem();
        if (selectedItem)
        {
            auto comboBoxItem = selectedItem.as<ComboBoxItem>();
            auto content = comboBoxItem.Content();
            auto contentString = content.as<hstring>();
            return WStringToString(std::wstring(contentString.c_str()));
        }
        return "1"; // Valor por defecto como string
    }

    std::wstring MainWindow::StringToWString(const std::string& str)
    {
        if (str.empty()) return std::wstring();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    std::string MainWindow::WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    std::vector<std::string> MainWindow::SplitString(const std::string& str, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);

        while (std::getline(tokenStream, token, delimiter))
        {
            // Eliminar espacios en blanco al inicio y final
            token.erase(0, token.find_first_not_of(" \t\n\r"));
            token.erase(token.find_last_not_of(" \t\n\r") + 1);
            tokens.push_back(token);
        }

        return tokens;
    }
}