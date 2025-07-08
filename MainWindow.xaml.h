#pragma once
#include "MainWindow.g.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <windows.h>

namespace winrt::DiscoSimulador::implementation
{
    // Estructura para metadata de ubicación física
    struct UbicacionFisica {
        int plateIndex;
        int surfaceIndex;
        int trackIndex;
        int sectorIndex;
        int charIndex;

        UbicacionFisica(int plate = 0, int surface = 0, int track = 0, int sector = 0, int charIdx = 0)
            : plateIndex(plate), surfaceIndex(surface), trackIndex(track),
            sectorIndex(sector), charIndex(charIdx) {
        }

        std::string ToString() const {
            return "Plato:" + std::to_string(plateIndex) +
                " Superficie:" + std::to_string(surfaceIndex) +
                " Pista:" + std::to_string(trackIndex) +
                " Sector:" + std::to_string(sectorIndex) +
                " Char:" + std::to_string(charIndex);
        }
    };

    // Estructura para metadata de un campo
    struct MetadataCampo {
        std::string nombreCampo;
        std::string valor;
        UbicacionFisica inicio;
        UbicacionFisica fin;

        MetadataCampo(const std::string& nombre, const std::string& val,
            const UbicacionFisica& ini, const UbicacionFisica & final)
            : nombreCampo(nombre), valor(val), inicio(ini), fin(final) {
        }
    };

    // Estructura para una fila con metadata
    struct FilaConMetadata {
        std::vector<std::string> datos;
        std::vector<MetadataCampo> metadata;

        FilaConMetadata(const std::vector<std::string>& d, const std::vector<MetadataCampo>& m)
            : datos(d), metadata(m) {
        }
    };

    struct ConfiguracionDisco {
        std::string numPlatos;
        std::string numSuperficies;
        std::string numPistas;

        ConfiguracionDisco(const std::string& platos = "1", const std::string& superficies = "2", const std::string& pistas = "100")
            : numPlatos(platos), numSuperficies(superficies), numPistas(pistas) {
        }
    };

    struct TablaDatos {
        std::vector<std::string> fields;
        std::vector<FilaConMetadata> data;

        TablaDatos(const std::vector<std::string>& f, const std::vector<FilaConMetadata>& d)
            : fields(f), data(d) {
        }
    };

    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        // Event handlers
        void InicializarButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void EjecutarButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ConsultaTextBox_KeyDown(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);

        // Nuevo event handler para el botón desplegable
        void ToggleMetadataButton_Click(Windows::Foundation::IInspectable const& sender,
            Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        // Nombres de archivos configurables
        std::string nombreArchivoEstructura = "struct_table.txt";
        std::string nombreArchivoCSV = "taxables.csv";

        // Miembros privados
        bool discoInicializado = false;
        ConfiguracionDisco configuracion;
        std::string estructuraTabla;
        std::vector<std::vector<std::string>> datosCSV;
        std::vector<std::string> camposCSV;

        // Simulador de ubicaciones físicas
        int currentPlate = 0;
        int currentSurface = 0;
        int currentTrack = 0;
        int currentSector = 0;
        int currentChar = 0;

        // Configuración adicional del disco
        std::string numSectores = "64";
        std::string tamanoSector = "512";

        // Métodos privados principales
        void EjecutarConsulta();
        void LimpiarResultados();
        void MostrarError(const std::string& mensaje);

        // Métodos para mostrar información
        void MostrarConfiguracion();
        void MostrarEstructura();
        void MostrarDatos();
        void MostrarConsultaEjecutada(const std::string& consulta);
        void MostrarTablaResultados(const TablaDatos& tabla);

        // Métodos para cargar archivos
        bool CargarEstructuraTabla();
        bool CargarDatosCSV();

        // Métodos para procesar consultas
        TablaDatos ProcesarConsulta(const std::string& consulta);
        std::vector<FilaConMetadata> BuscarCoincidencias(const std::string& consulta);

        // Métodos para simular ubicaciones físicas
        UbicacionFisica CalcularUbicacionInicio(const std::string& valor);
        UbicacionFisica CalcularUbicacionFin(const UbicacionFisica& inicio, const std::string& valor);
        void AvanzarPosicionDisco(int caracteres);
        FilaConMetadata CrearFilaConMetadata(const std::vector<std::string>& fila);

        // Métodos para crear elementos de UI
        Microsoft::UI::Xaml::Controls::Border CrearFilaTabla(const std::vector<std::string>& fila, bool esEncabezado = false);
        Microsoft::UI::Xaml::Controls::Border CrearFilaTablaConMetadata(const FilaConMetadata& fila, bool esEncabezado = false);
        Microsoft::UI::Xaml::Controls::StackPanel CrearPanelMetadata(const MetadataCampo& metadata);

        // Métodos para obtener valores de los ComboBox
        std::string ObtenerValorComboBox(Microsoft::UI::Xaml::Controls::ComboBox const& comboBox);

        // Métodos de utilidad
        std::wstring StringToWString(const std::string& str);
        std::string WStringToString(const std::wstring& wstr);
        std::vector<std::string> SplitString(const std::string& str, char delimiter);
    };
}

namespace winrt::DiscoSimulador::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}