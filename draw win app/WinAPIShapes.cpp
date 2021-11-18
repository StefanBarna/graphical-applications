#include <Windows.h>
#include <gdiplus.h>
#include <Uxtheme.h> // draw with windows styles
#include <strsafe.h>
#include <list>
#include <algorithm>
#include <sstream>
#include "resource.h"
#include "Shape.h"
#include "Circle.h"
#include "Utilities.h"
#include "WinAPIShapes.h"
#include "WinAPISettings.h"

using namespace Gdiplus;
using namespace std;

extern Circle animatedCircle;

WinAPIShapes::MessageMap WinAPIShapes::ms_msgMap[100]{};
size_t WinAPIShapes::ms_cnt{};

WinAPIShapes* WinAPIShapes::ms_pWnd[100]{};
size_t WinAPIShapes::ms_wndCnt{};

WinAPIShapes* WinAPIShapes::findWindow(HWND hWnd) {
    for (size_t i = 0; i < WinAPIShapes::ms_wndCnt; ++i) {
        if (WinAPIShapes::ms_pWnd[i]->m_hWnd == hWnd)
            return WinAPIShapes::ms_pWnd[i];
    }
    return nullptr;
}

LRESULT CALLBACK WinAPIShapes::WndProcClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static std::stringstream ss;
    ss << hex << message << "\n";
    // find the window
    WinAPIShapes* wnd = WinAPIShapes::findWindow(hWnd);

    if (wnd == nullptr)
    {
        //if (message == WM_GETMINMAXINFO)
        //{
        //    ss << hex << message << "\n";
        //}
        //if (message == WM_NCCREATE)
        //{
        //    ss << hex << message << "\n";
        //}
        //if (message == WM_NCDESTROY)
        //{
        //    ss << hex << message << "\n";
        //}
        //if (message == WM_NCCALCSIZE)
        //{
        //    ss << hex << message << "\n";
        //}
        if (message == WM_CREATE)
        {
            ss << hex << message << "\n";
            CREATESTRUCT* lCreate = (CREATESTRUCT*)lParam;
            wnd = (WinAPIShapes*)lCreate->lpCreateParams;
            wnd->m_hWnd = hWnd;
        }
    }

    if (wnd != nullptr)
        return wnd->WndProc(message, wParam, lParam);

    return DefWindowProc(hWnd, message, wParam, lParam);
};

HINSTANCE WinAPIShapes::ms_hInstance{};

bool WinAPIShapes::ms_isRegistered = false;

WinAPIShapes::WinAPIShapes()
{
    // if no the class is not registered  -> register everything
    if (!WinAPIShapes::ms_isRegistered) {
        WinAPIShapes::staticConstructor();
        this->registerClass();
        WinAPIShapes::ms_isRegistered = true;
    }


    this->m_filename = "shapes.txt";
}

void WinAPIShapes::staticConstructor() {
	// onPaint
    WinAPIShapes::hReg(WM_PAINT, &WinAPIShapes::onPaint);

    // onCreate
    WinAPIShapes::hReg(WM_CREATE, &WinAPIShapes::onCreate);

    // onDestroy
    WinAPIShapes::hReg(WM_DESTROY, &WinAPIShapes::onDestroy);

    // onLButtonDown
    WinAPIShapes::hReg(WM_LBUTTONDOWN, &WinAPIShapes::onLButtonDown);

    // onLButtonUp
    WinAPIShapes::hReg(WM_LBUTTONUP, &WinAPIShapes::onLButtonUp);

    // onMouseMove
    WinAPIShapes::hReg(WM_MOUSEMOVE, &WinAPIShapes::onMouseMove);

    // onMouseWheel
    WinAPIShapes::hReg(WM_MOUSEWHEEL, &WinAPIShapes::onMouseWheel);

    // onCommand
    WinAPIShapes::hReg(WM_COMMAND, &WinAPIShapes::onCommand);
}

void WinAPIShapes::hReg(UINT message, MessageHandler hMsg) {
    WinAPIShapes::ms_msgMap[WinAPIShapes::ms_cnt].msg = message;
    WinAPIShapes::ms_msgMap[WinAPIShapes::ms_cnt].pfnHandler = hMsg;
    WinAPIShapes::ms_cnt++;
}

LRESULT CALLBACK WinAPIShapes::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	for (size_t i = 0; i < WinAPIShapes::ms_cnt; ++i) {
		if (message == WinAPIShapes::ms_msgMap[i].msg) {
			// calling the pfnHandler on the current instance (this) with wParam and lParam
			(this->*WinAPIShapes::ms_msgMap[i].pfnHandler)(wParam, lParam);
			break;
		}
	}
    return  DefWindowProc(this->m_hWnd, message, wParam, lParam);
}

void WinAPIShapes::onPaint(WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(this->m_hWnd, &ps);

    RECT rc;
    ::GetClientRect(this->m_hWnd, &rc); // puts a rectangle encapsulating the whole client area into the second parameter
    HDC memDc;
    auto hbuf = ::BeginBufferedPaint(hdc, &rc, BPBF_COMPATIBLEBITMAP, nullptr, &memDc);

    // if hbuf is not 0, do double buffering, otherwise do single buffering
    Graphics graphics(memDc);
    graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
    graphics.Clear(Color::AliceBlue);

    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
        (*it)->draw(graphics);
    }
    animatedCircle.draw(graphics);

    ::EndBufferedPaint(hbuf, true);
    EndPaint(this->m_hWnd, &ps);
    ReleaseDC(this->m_hWnd, memDc); // check if this is correct, might not be
    ReleaseDC(this->m_hWnd, hdc);
}

void WinAPIShapes::onCreate(WPARAM wParam, LPARAM lParam) {
    RECT rect;
    // GetWindowRect(); // includes title bar, menu bar, etc.
    GetClientRect(this->m_hWnd, &rect);

    this->m_lbhWnd = CreateWindow(WC_LISTBOX, TEXT("Mouse Events"),
        WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY | LBS_HASSTRINGS,
        10, rect.bottom - 10 - 100, rect.right - 20, 100,
        this->m_hWnd, NULL, (HINSTANCE)GetWindowLongPtr(this->m_hWnd, GWLP_HINSTANCE), NULL);

    this->loadFile();
}

void WinAPIShapes::onDestroy(WPARAM wParam, LPARAM lParam) {
    this->saveFile();
    PostQuitMessage(0);
}

void WinAPIShapes::onLButtonDown(WPARAM wParam, LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // check if the mouse is on a circle
    bool overlap = isOnShape(x, y);
    if (this->m_selected)
        this->selectShape();
    else {
        // if control is being pressed, create a square instead
        if (MK_CONTROL == (MK_CONTROL & wParam)) {
            if (!overlap)
                this->createShape<Square>(x, y);
        }
        // otherwise create a circle
        else {
            if (!overlap)
                this->createShape<Circle>(x, y);
        }
    }
}

void WinAPIShapes::onLButtonUp(WPARAM wParam, LPARAM lParam) {
    if (this->m_selected)
        this->releaseShape(lParam);
}

void WinAPIShapes::onMouseMove(WPARAM wParam, LPARAM lParam) {
    if (this->m_selected)
        this->moveShape(lParam);
}

void WinAPIShapes::onMouseWheel(WPARAM wParam, LPARAM lParam) {
    if (this->m_selected)
        this->resizeShape(lParam, wParam);
}

void WinAPIShapes::onCommand(WPARAM wParam, LPARAM lParam) {
    int wmId = LOWORD(wParam);
    // Parse the menu selections:
    switch (wmId)
    {
        /*case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;*/
    case ID_EDIT_SETTINGS:
    {
        WinAPISettings::staticConstructor();
        WinAPISettings wndSetting;
        wndSetting.create(this->m_hWnd);
        // grab the data from object
        break;
    }
    case IDM_EXIT:
        DestroyWindow(this->m_hWnd);
        break;
    }
}

void WinAPIShapes::loadFile() {
    this->m_file.open(this->m_filename);

    // read from the file
    if (this->m_file.is_open()) {
        int size;
        this->m_file >> size;
        this->m_file.ignore(1);

        for (int i = 0; i < size; i++) {
            char iden = this->m_file.peek();
            Shape* s = shapeFactory(iden);

            s->load(this->m_file);

            this->m_shapes.push_back(s);
        }
    }

    this->m_file.close();
}

void WinAPIShapes::saveFile() {
    // open a file
    this->m_file.open(this->m_filename);

    // edit the file
    if (this->m_file.is_open()) {
        this->m_file << Shape::shapeCnt() << endl;
        for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
            (*it)->save(this->m_file);
        }
    }

    // close the file
    this->m_file.close();
}

void WinAPIShapes::selectShape() {
    this->m_selectedx = this->m_selected->getX();
    this->m_selectedy = this->m_selected->getY();

    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("x = [%d], y = [%d] -- Clicked inside circle!"), this->m_selected->getX(), this->m_selected->getY());
    SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
}

void WinAPIShapes::moveShape(LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // save old locations
    int old_x = this->m_selected->getX();
    int old_y = this->m_selected->getY();

    this->m_selected->setPos(x, y);

    // redraw the window
    RECT newArea{ x - (this->m_selected->getWidth() / 2) - 3, y - (this->m_selected->getWidth() / 2) - 3, 
        x + (this->m_selected->getWidth() / 2) + 3, y + (this->m_selected->getWidth() / 2) + 3 }; // gets the attributes one by one
    RECT oldArea{ old_x - (this->m_selected->getWidth() / 2) - 3, old_y - (this->m_selected->getWidth() / 2) - 3, 
        old_x + (this->m_selected->getWidth() / 2) + 3, old_y + (this->m_selected->getWidth() / 2) + 3 };
    RECT* totalArea = this->fusedRect(newArea, oldArea);
    InvalidateRect(this->m_hWnd, totalArea, false);
    delete totalArea;
}

void WinAPIShapes::releaseShape(LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    TCHAR buffer[128]{};
    StringCchPrintf(buffer, 128, TEXT("Shape wants to move from x = [%d], y = [%d] to x = [%d], y = [%d]"), this->m_selected->getX(), this->m_selected->getY(), x, y);
    SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)buffer);

    // check for overlapping
    bool overlap = false;
    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end() && !overlap; it++) {
        overlap = (*it)->overlap(*this->m_selected);
    }

    // if there's no overlap, place the shape
    if (!overlap) {
        this->m_selected->setPos(x, y);
        SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Shape moved successfully!"));
    }

    // if there is overlap, return shape to original position
    else {
        this->m_selected->setPos(this->m_selectedx, this->m_selectedy);
        SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Shape overlapping with a preexisting shape! Not moved."));

        // redraw area with circle
        RECT newArea{ this->m_selected->getX() - (this->m_selected->getWidth() / 2) - 3, this->m_selected->getY() - (this->m_selected->getWidth() / 2) - 3, 
            this->m_selected->getX() + (this->m_selected->getWidth() / 2) + 3, this->m_selected->getY() + (this->m_selected->getWidth() / 2) + 3 };
        InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }

    // redraw area with circle
    RECT newArea{ x - (this->m_selected->getWidth() / 2) - 3, y - (this->m_selected->getWidth() / 2) - 3, 
        x + (this->m_selected->getWidth() / 2) + 3, y + (this->m_selected->getWidth() / 2) + 3 };
    InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen

    this->m_selected = nullptr;
    this->m_selectedx = -1;
    this->m_selectedy = -1;
}

void WinAPIShapes::resizeShape(LPARAM lParam, WPARAM wParam) {
    int scrollQuant = HIWORD(wParam);
    int sizeChange;

    if (scrollQuant == 120) // scroll away
        sizeChange = 4;
    else if (this->m_selected->getWidth() > 32)
        sizeChange = -4;
    else
        sizeChange = 0;

    // get old width
    int oldWidth = this->m_selected->getWidth();

    // change width
    this->m_selected->setWidth(this->m_selected->getWidth() + sizeChange);

    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // keeps x and y in the center
    x -= sizeChange / 2;
    y += sizeChange / 2;

    if (sizeChange > 0) {
        // redraw area with circle
        RECT newArea{ this->m_selected->getX() - (this->m_selected->getWidth() / 2) - 3, this->m_selected->getY() - (this->m_selected->getWidth() / 2) - 3, this->m_selected->getX() + (this->m_selected->getWidth() / 2) + 3, this->m_selected->getY() + (this->m_selected->getWidth() / 2) + 3 };
        InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }
    else {
        // redraw area with circle
        RECT newArea{ this->m_selected->getX() - (oldWidth / 2) - 3, this->m_selected->getY() - (oldWidth / 2) - 3, this->m_selected->getX() + (oldWidth / 2) + 3, this->m_selected->getY() + (oldWidth / 2) + 3 };
        InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
    }
}

bool WinAPIShapes::isOnShape(int x, int y) {
    bool onShape = false;
    Shape* s = new Square(x, y, Shape::defaultWidth, false); // square because it's a hitbox

    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end() && !this->m_selected && !onShape; it++) {
        onShape = (*it)->overlap(*s);

        if (onShape == true)
            this->m_selected = *it;
    }

    delete s;
    return onShape;
}

RECT* WinAPIShapes::fusedRect(RECT r1, RECT r2) {
    RECT* fused = new RECT{};

    fused->left = min(r1.left, r2.left);
    fused->top = min(r1.top, r2.top);
    fused->right = max(r1.right, r2.right);
    fused->bottom = max(r1.bottom, r2.bottom);

    return fused;
}

void WinAPIShapes::setWindowTitle(const TCHAR* title) {
    StringCchPrintf(this->m_windowTitle, sizeof(this->m_windowTitle)/sizeof(this->m_windowTitle[0]), title);
}

BOOL WinAPIShapes::create(int nCmdShow)
{
    //this->m_hInst = hInstance; // Store instance handle in our global variable

    WinAPIShapes::ms_pWnd[WinAPIShapes::ms_wndCnt] = this;
    ++WinAPIShapes::ms_wndCnt;

    this->m_hWnd = CreateWindowEx(
                                    0,                                                                      /* extended styles */
                                    WinAPIShapes::ms_szWindowClass,                                         /*  which windows to create -- must be registered before*/
                                    this->m_windowTitle,                                                    /* text that appear in the title bar of the window */
                                    WS_OVERLAPPEDWINDOW,                                                    /* styles used to create */
                                    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,                                     /* position on screen */
                                    nullptr,                                                                /* the parent of this window */
                                    nullptr,                                                                /* handle to the menu */
                                    WinAPIShapes::ms_hInstance,                                             /* handle of the module */
                                    this                                                                    /* LPARAM, passed to WM_CREATE */
                                );

    if (!this->m_hWnd)
    {
        return FALSE;
    }

    ShowWindow(this->m_hWnd, nCmdShow);
    UpdateWindow(this->m_hWnd);

    return TRUE;
}
