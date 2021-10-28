// draw win app.cpp : Defines the entry point for the application.
//

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

// List of circles
std::list<Circle> circles;

// list of shapes
std::list<Shape*> shapes;

// Circle that's being moved
Shape* selected = nullptr;
// original x and y values for the shape being moved
int selectedX = -1, selectedY = -1;

// File name
string fileName = "shapes.txt";
fstream shapeFile;

// global instance of a circle
Circle animatedCircle(100, 100, Shape::defaultWidth, false);

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SettingsWndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize GDI+
    ULONG_PTR m_gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    ::BufferedPaintInit();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DRAWWINAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
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

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAWWINAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DRAWWINAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// calculates distance between two points
double distance(double x1, double y1, double x2, double y2) {
    double dist = (y1 - y2) * (y1 - y2);
    dist += (x1 - x2) * (x1 - x2);
    dist = sqrt(dist);
    return dist;
}

void onPaint(HWND hWnd, WPARAM wParam, LPARAM lParam) {

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rc;
    ::GetClientRect(hWnd, &rc); // puts a rectangle encapsulating the whole client area into the second parameter
    HDC memDc;
    auto hbuf = ::BeginBufferedPaint(hdc, &rc, BPBF_COMPATIBLEBITMAP, nullptr, &memDc);

    Graphics graphics(memDc);
    graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
    graphics.Clear(Color::AliceBlue);

    for (auto it = shapes.begin(); it != shapes.end(); it++) {
        (*it)->draw(graphics);
    }
    animatedCircle.draw(graphics); // TODO: is not drawn

    ::EndBufferedPaint(hbuf, true);
    EndPaint(hWnd, &ps);
    ReleaseDC(hWnd, memDc); // check if this is correct, might not be
    ReleaseDC(hWnd, hdc);
}

// moves a circle to a target location
void moveShape(HWND hWnd, LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // save old locations
    int old_x = selected->getX();
    int old_y = selected->getY();

    selected->setPos(x, y);

    // redraw the window
    RECT newArea{ x - (selected->getWidth() / 2) - 3, y - (selected->getWidth() / 2) - 3, x + (selected->getWidth() / 2) + 3, y + (selected->getWidth() / 2) + 3 }; // gets the attributes one by one
    RECT oldArea{ old_x - (selected->getWidth() / 2) - 50, old_y - (selected->getWidth() / 2) - 50, old_x + (selected->getWidth() / 2) + 50, old_y + (selected->getWidth() / 2) + 50 }; // TODO: one area
    // InvalidateRect(hWnd, &newArea, false); // only modifies a certain rectangle of the screen
    InvalidateRect(hWnd, &oldArea, false); // TODO: InvalidateRgn https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-invalidatergn
}

// lets go of the selected circle and places it at the location of the lParam
void releaseShape(HWND hWnd, HWND lbhwnd, LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("Circle wants to move from x = [%d], y = [%d] to x = [%d], y = [%d]"), selected->getX(), selected->getY(), x, y);
    SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)buffer);

    // check for overlapping
    bool overlap = false;
    for (auto it = shapes.begin(); it != shapes.end() && !overlap; it++) {
        overlap = (*it)->overlap(*selected);
    }

    // if there's no overlap, place the shape
    if (!overlap) {
        selected->setPos(x, y);
        SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Circle moved successfully!"));
    }

    // if there is overlap, return shape to original position
    else {
        selected->setPos(selectedX, selectedY);
        SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Circle overlapping with a preexisting circle! Not moved."));

        // redraw area with circle
        RECT newArea{ selected->getX() - (selected->getWidth() / 2) - 3, selected->getY() - (selected->getWidth() / 2) - 3, selected->getX() + (selected->getWidth() / 2) + 3, selected->getY() + (selected->getWidth() / 2) + 3 };
        InvalidateRect(hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }
    // redraw area with circle
    RECT newArea{ x - (selected->getWidth() / 2) - 3, y - (selected->getWidth() / 2) - 3, x + (selected->getWidth() / 2) + 3, y + (selected->getWidth() / 2) + 3 };
    InvalidateRect(hWnd, &newArea, true); // only modifies a certain rectangle of the screen

    selected = nullptr;
    selectedX = -1;
    selectedY = -1;
}

// resizes the circle based on the amount the user scrolls
void resizeShape(HWND hWnd, LPARAM lParam, WPARAM wParam) {
    int scrollQuant = HIWORD(wParam);
    int sizeChange;

    if (scrollQuant == 120) // scroll away
        sizeChange = 4;
    else if (selected->getWidth() > 32)
        sizeChange = -4;
    else
        sizeChange = 0;

    // get old width
    int oldWidth = selected->getWidth();

    // change width
    selected->setWidth(selected->getWidth() + sizeChange);

    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // keeps x and y in the center
    x -= sizeChange / 2;
    y += sizeChange / 2;
    
    if (sizeChange > 0) {
        // redraw area with circle
        RECT newArea{ selected->getX() - (selected->getWidth() / 2) - 3, selected->getY() - (selected->getWidth() / 2) - 3, selected->getX() + (selected->getWidth() / 2) + 3, selected->getY() + (selected->getWidth() / 2) + 3 };
        InvalidateRect(hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }
    else {
        // redraw area with circle
        RECT newArea{ selected->getX() - (oldWidth / 2) - 3, selected->getY() - (oldWidth / 2) - 3, selected->getX() + (oldWidth / 2) + 3, selected->getY() + (oldWidth / 2) + 3 };
        InvalidateRect(hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }
}

// saves all circles to a file
void saveFile() {
    // open a file
    shapeFile.open(fileName);

    // edit the file
    if (shapeFile.is_open()) {
        shapeFile << Shape::shapeCnt() << endl;
        for (auto it = shapes.begin(); it != shapes.end(); it++) {
            (*it)->save(shapeFile);
        }
    }

    // close the file
    shapeFile.close();
}

// loads all circles from a file
void openFile() {
    shapeFile.open(fileName);

    // read from the file
    if (shapeFile.is_open()) {
        int size;
        shapeFile >> size;
        shapeFile.ignore(1);

        for (int i = 0; i < size; i++) {
            char iden = shapeFile.peek();
            Shape* s = shapeFactory(iden);

            s->load(shapeFile);
            
            shapes.push_back(s);
        }
    }

    shapeFile.close();
}

// checks if the location x, y is on a circle
bool isOnShape(int x, int y) {
    bool onShape = false;
    Shape* s = new Square(x, y, Shape::defaultWidth, false);

    for (auto it = shapes.begin(); it != shapes.end() && !selected && !onShape; it++) {
        onShape = (*it)->overlap(*s);

        if (onShape == true)
            selected = *it;
    }

    delete s;
    return onShape;
}

// select a shape to move
void selectMovingShape(HWND lbhwnd) {
    selectedX = selected->getX();
    selectedY = selected->getY();

    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("x = [%d], y = [%d] -- Clicked inside circle!"), selected->getX(), selected->getY());
    SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
}

// create a new circle at location x, y
void createCircle(HWND hWnd, HWND lbhwnd, int x, int y) {
    // create a circle
    int width = Shape::defaultWidth;
    Circle* c = new Circle(x, y);

    // save the circle to the list of shapes
    shapes.push_back(c);

    // redraw the area in which you made the shape
    RECT shapeArea{ x - (c->getWidth() / 2) - 3, y - (c->getWidth() / 2) - 3, x + (c->getWidth() / 2) + 3, y + (c->getWidth() / 2) + 3 }; // gets the attributes one by one
    InvalidateRect(hWnd, &shapeArea, true); // only modifies a certain rectangle of the screen

    // add a new element in the list (a new string)
    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("%d. x = [%d], y = [%d]  --  Added a new circle with ID [%d]!"), shapes.size(), c->getX(), c->getY(), c->getID());
    SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
}

// create a new circle at location x, y
void createSquare(HWND hWnd, HWND lbhwnd, int x, int y) {
    // create a circle
    int width = Shape::defaultWidth;
    Square* s = new Square(x, y);

    // save the circle to the list of shapes
    shapes.push_back(s);

    // redraw the area in which you made the shape
    RECT shapeArea{ x - (s->getWidth() / 2) - 3, y - (s->getWidth() / 2) - 3, x + (s->getWidth() / 2) + 3, y + (s->getWidth() / 2) + 3 }; // gets the attributes one by one
    InvalidateRect(hWnd, &shapeArea, true); // only modifies a certain rectangle of the screen

    // add a new element in the list (a new string)
    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("%d. x = [%d], y = [%d]  --  Added a new square with ID [%d]!"), shapes.size(), s->getX(), s->getY(), s->getID());
    SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cnt = 0;
    static HWND lbhwnd;
    switch (message)
    {
    case WM_CREATE:
        {
            RECT rect;
            // GetWindowRect(); // includes title bar, menu bar, etc.
            GetClientRect(hWnd, &rect);

            // TODO: look in the documentation at this funtion
            lbhwnd = CreateWindow(WC_LISTBOX, TEXT("Mouse Events"),
                WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY | LBS_HASSTRINGS,
                10, rect.bottom - 10 - 100, rect.right - 20, 100,
                hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

            // recreate the list of circles from the file
            openFile();

            // set the timer
            SetTimer(hWnd, 1, 20, nullptr);
        }
        break;
    case WM_TIMER:
    {
        animatedCircle.setX(animatedCircle.getX() + 5);
        // TODO: invalidate rect function
        //RECT shapeArea{animatedCircle.getX() - (animatedCircle.getWidth() / 2) - 3, animatedCircle.getY() - (animatedCircle.getWidth() / 2) - 3, 
        //    animatedCircle.getX() + (animatedCircle.getWidth() / 2) + 3, animatedCircle.getY() + (animatedCircle.getWidth() / 2) + 3 };
        //InvalidateRect(hWnd, &shapeArea, true); // only modifies a certain rectangle of the screen
        RECT newArea{ animatedCircle.getX() - (animatedCircle.getWidth() / 2) - 3, animatedCircle.getY() - (animatedCircle.getWidth() / 2) - 3,
            animatedCircle.getX() + (animatedCircle.getWidth() / 2) + 3, animatedCircle.getY() + (animatedCircle.getWidth() / 2) + 3 }; // gets the attributes one by one
        RECT oldArea{ animatedCircle.getX() - 5 - (animatedCircle.getWidth() / 2) - 3, animatedCircle.getY() - (animatedCircle.getWidth() / 2) - 3,
            animatedCircle.getX() - 5 + (animatedCircle.getWidth() / 2) + 3, animatedCircle.getY() + (animatedCircle.getWidth() / 2) + 3 };
        InvalidateRect(hWnd, &newArea, true); // only modifies a certain rectangle of the screen
        InvalidateRect(hWnd, &oldArea, true);

        TCHAR buffer[128]{};
        StringCchPrintf(buffer, 128, TEXT("One second has passed!"));
        //SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_EDIT_SETTINGS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, SettingsWndProc);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_APPLY_SETTINGS: {
        TCHAR* text = reinterpret_cast<TCHAR*>(wParam);
        TCHAR buffer[128]{};
        StringCchPrintf(buffer, 128, TEXT("Send the following greetings: %s"), text);
        SendMessage(lbhwnd, LB_INSERTSTRING, 0, (LPARAM)text);
        delete[] text;
        break;
    }
    case WM_LBUTTONDOWN:
        {
            int x, y;
            x = LOWORD(lParam);
            y = HIWORD(lParam);

            // check if the mouse is on a circle
            bool overlap = isOnShape(x, y);
            if (selected) {
                selectMovingShape(lbhwnd);
            }

            // if control is being pressed, create a square instead
            if (MK_CONTROL == (MK_CONTROL & wParam)) {
                if (!overlap) {
                    createSquare(hWnd, lbhwnd, x, y);
                }
            }
            // otherwise create a circle
            else {
                if (!overlap) {
                    createCircle(hWnd, lbhwnd, x, y);
                }
            }
        }
        break;
    case WM_LBUTTONUP:
        if (selected)
            releaseShape(hWnd, lbhwnd, lParam);
        break;
    case WM_MOUSEMOVE:
        if (selected)
            moveShape(hWnd, lParam);
        break;
    case WM_MOUSEWHEEL:
        if (selected)
            resizeShape(hWnd, lParam, wParam);
        break;
    case WM_PAINT:
        // TODO: look up double buffering (create a device context for a bitmap, and print it all at once to the screen)
        // CreateCompatibleBitmap()
        // CreateCompatibleDC()
        // BitBLT()

        onPaint(hWnd, lParam, wParam);
        break;
    case WM_DESTROY: // when the user closes the program
        saveFile();
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
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

// Message handler for settings box
INT_PTR CALLBACK SettingsWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            //::MessageBox(hDlg, TEXT("Saving the settings..."), TEXT("Saved"), MB_OK);
            auto parentWnd = ::GetParent(hDlg);

            // receive information from text areas
            TCHAR* widthBuffer = new TCHAR[128]{};
            TCHAR* redBuffer = new TCHAR[128]{};
            TCHAR* greenBuffer = new TCHAR[128]{};
            TCHAR* blueBuffer = new TCHAR[128]{};
            ::GetDlgItemText(hDlg, IDC_WIDTH_INPUT, widthBuffer, 128);
            ::GetDlgItemText(hDlg, IDC_RED_INPUT, redBuffer, 128);
            ::GetDlgItemText(hDlg, IDC_GREEN_INPUT, greenBuffer, 128);
            ::GetDlgItemText(hDlg, IDC_BLUE_INPUT, blueBuffer, 128);
            // TODO: use SetDlgItemText() to include the already existing values

            // convert it to readable information
            if (widthBuffer[0] != '\0') {
                int width = _wtoi(widthBuffer);
                Shape::defaultWidth = width;
            }
            if (redBuffer[0] != '\0' && redBuffer[0] != '\0' && redBuffer[0] != '\0') {
                int red = _wtoi(redBuffer);
                int green = _wtoi(greenBuffer);
                int blue = _wtoi(blueBuffer);
                Shape::defaultBorderColour = Color(red, green, blue);
            }

            // delete tchar strings
            delete[] widthBuffer;
            delete[] redBuffer;
            delete[] greenBuffer;
            delete[] blueBuffer;

            //::MessageBox(hDlg, buffer, TEXT("Input"), MB_OK);
            //::PostMessage(parentWnd, WM_APPLY_SETTINGS, reinterpret_cast<WPARAM>(idBuffer), NULL);
            break;
        }

        if (LOWORD(wParam) == IDOK or LOWORD(wParam) == IDCANCEL)
        {
            ::EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        break;
    }
    return (INT_PTR)FALSE;
}

// TODO: add a border to selected shape
// TODO: rework selection

// TODO: get rid of flickering through InvalidateRect
// TODO: delete circle when you click delete

// TODO: modify the graphical interface to select colour and size of circles
// TODO: make each circle size configurable through the scroll wheel (scroll wheel message)

// TODO: add more information about each circle
// TODO: add functions to the circle structure (draw, isOverlapping)
// TODO: (!) split code into functions
// TODO: circles move automatically
// TODO: circles bounce on contact with borders
// TODO: double buffering

// TODO: GDI+