#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>
#include "ListRegister.hh"  // usa tus propias clases

#pragma comment(lib, "comctl32.lib")

class App {
private:
    HWND hWnd = nullptr;
    HWND hTab, hEdit, hButton, hListView;
    ListRegister lista;
    std::wstring currentQuery;

public:
    App() : lista({ "ID", "Item", "Cost", "Tax", "Total" }) {}

    void Run(HINSTANCE hInstance, int nCmdShow) {
        const wchar_t CLASS_NAME[] = L"VentanaPrincipal";

        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.cbWndExtra = sizeof(App*);

        RegisterClass(&wc);

        hWnd = CreateWindowEx(0, CLASS_NAME, L"Consulta de Productos",
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 560, 360,
            NULL, NULL, hInstance, this);

        if (!hWnd) return;

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    void InsertarDatos() {
        wchar_t buffer[256];
        GetWindowTextW(hEdit, buffer, 256);
        //currentQuery = buffer; // guarda

        //  Ejemplo: insertar una fila fija
        lista.add({ "1", "Notebook", "12.50", "1.00", "13.50" });

        TabCtrl_SetCurSel(hTab, 1);
        ShowWindow(hEdit, SW_HIDE);
        ShowWindow(hButton, SW_HIDE);
        MoveWindow(hListView, 20, 60, 470, 200, TRUE);

        ListView_DeleteAllItems(hListView);

        for (int i = 0; i < lista.size(); ++i) {
            LVITEM item = { 0 };
            item.mask = LVIF_TEXT;
            item.iItem = i;

            std::string field0 = lista[i].fields[0];
            std::wstring text0(field0.begin(), field0.end());
            item.pszText = const_cast<LPWSTR>(text0.c_str());
            ListView_InsertItem(hListView, &item);

            for (int j = 1; j < lista[i].fields.size(); ++j) {
                std::wstring text(lista[i].fields[j].begin(), lista[i].fields[j].end());
                ListView_SetItemText(hListView, i, j, const_cast<LPWSTR>(text.c_str()));
            }
        }
    }
    void InicializarTabla(HWND hwnd) {
        LVCOLUMN col;
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.cx = 100;

        int index = 0;
        for (const std::string& name : lista.getFieldNames()) {
            std::wstring header(name.begin(), name.end());
            col.pszText = const_cast<LPWSTR>(header.c_str());
            ListView_InsertColumn(hListView, index, &col);
            ++index;
        }
    }



    void OnCreate(HWND hwnd) {
        INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES };
        InitCommonControlsEx(&icex);

        hTab = CreateWindowEx(0, WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE,
            10, 10, 500, 30, hwnd, (HMENU)1000, NULL, NULL);

        TCITEM tie = { TCIF_TEXT, 0, 0, (LPWSTR)L"Insertar", 0, -1, 0 };
        TabCtrl_InsertItem(hTab, 0, &tie);
        tie.pszText = (LPWSTR)L"Resultados";
        TabCtrl_InsertItem(hTab, 1, &tie);

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 60, 300, 25, hwnd, (HMENU)1001, NULL, NULL);

        hButton = CreateWindow(L"BUTTON", L"Enviar",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            330, 60, 80, 25, hwnd, (HMENU)1002, NULL, NULL);

        hListView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"",
            WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
            20, 60, 470, 200, hwnd, (HMENU)1003, NULL, NULL);

        MoveWindow(hListView, -1000, -1000, 0, 0, TRUE);
        InicializarTabla(hwnd);

    }

    void OnTabChange() {
        int sel = TabCtrl_GetCurSel(hTab);
        if (sel == 0) {
            // Mostrar controles de insercion
            MoveWindow(hListView, -1000, -1000, 0, 0, TRUE);
            ShowWindow(hEdit, SW_SHOW);
            ShowWindow(hButton, SW_SHOW);
            EnableWindow(hEdit, TRUE);
            EnableWindow(hButton, TRUE);
        }
        else {
            // Mostrar tabla
            ShowWindow(hEdit, SW_HIDE);
            ShowWindow(hButton, SW_HIDE);
            EnableWindow(hEdit, FALSE);
            EnableWindow(hButton, FALSE);
            MoveWindow(hListView, 20, 60, 470, 200, TRUE);
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        App* self = nullptr;
        if (msg == WM_NCCREATE) {
            CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
            self = (App*)cs->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            self->hWnd = hwnd;
        }
        else {
            self = (App*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }

        if (self) {
            switch (msg) {
            case WM_CREATE:
                self->OnCreate(hwnd);
                break;

            case WM_COMMAND:
                if (LOWORD(wParam) == 1002) { // IDC_BUTTON
                    self->InsertarDatos();
                }
                break;

            case WM_NOTIFY:
                if (((LPNMHDR)lParam)->idFrom == 1000 && ((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
                    self->OnTabChange();
                }
                break;

            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            }
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
};
