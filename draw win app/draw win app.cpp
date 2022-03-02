// draw win app.cpp : Defines the entry point for the application.

// TODO: vectors
// TODO: client area : WM_NCCALCSIZE
// TODO: heirarchy for window classes
// TODO: resize window : look into anchoring

// TODO: start and stop buttons for the animation
// TODO: RESEARCH how to modify the client area to make room for other visual element

// TODO: file save, save as, and load in the file menu
//       open and save as dialog boxes
// https://docs.microsoft.com/en-us/windows/win32/dlgbox/open-and-save-as-dialog-boxes
// - &Save
// - Save &As ...
// - &Open ...
// TODO: fill / gradient shapes
//
// OutputDebugString

// TODO: speed control via a slider

// TODO: allow each message to send its own return type
// TODO: create custom control for selecting colour in settings
// TODO: make this control a class

#include <list>
#include <Windows.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <gdiplus.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <Uxtheme.h> // draw with windows styles
#include "framework.h"
#include "draw win app.h"

#include "Shape.h"
#include "Circle.h"
#include "Utilities.h"
#include "WinAPIShapes.h"
#include "WinAPISettings.h"
#include "CustomCtrl.h"

using namespace Gdiplus;
using namespace std;
// pragma comment links an executable to a library
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib, "Uxtheme.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100

const DWORD WM_APPLY_SETTINGS = WM_USER + 0x0001;

// global instance of WinAPIShapes
WinAPIShapes wnd;

// Global Variables:
HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//WinAPIShapes wnd;

// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SettingsWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CustomCtrl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    RegisterControl(hInstance);

    // TODO: Place code here.
    WinAPIShapes::staticConstructor();

    // Initialize GDI+
    ULONG_PTR m_gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    ::BufferedPaintInit();

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_DRAWWINAPP, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);

    WinAPIShapes wnd;
    wnd.setWindowTitle(TEXT("Shapes Application"));

    // Perform application initialization:
    //if (!wnd.create(nCmdShow))
    // Attempt to attach an Instace of type WinAPIShapes to a window in OS
    if (!wnd.create(nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAWWINAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    ::BufferedPaintUnInit();
    // shut down GDI+
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    // unregister the class
    UnregisterClass(TEXT("MyCustomCtrl"), NULL);

    return (int) msg.wParam;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

////LRESULT CALLBACK CustomCtrl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_CREATE:
//        return (INT_PTR)TRUE;
//
//    case WM_SETFONT:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    case WM_PAINT: {}
//    }
//    return (INT_PTR)FALSE;
//}