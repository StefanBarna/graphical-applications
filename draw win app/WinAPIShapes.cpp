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

    this->m_filename = L"shapes.txt";
}

//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM WinAPIShapes::registerClass() const
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WinAPIShapes::WndProcClass;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = WinAPIShapes::ms_hInstance;
    wcex.hIcon = LoadIcon(WinAPIShapes::ms_hInstance, MAKEINTRESOURCE(IDI_DRAWWINAPP));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DRAWWINAPP);
    wcex.lpszClassName = WinAPIShapes::ms_szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    auto res = RegisterClassExW(&wcex);

    return res;
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

    // onRButtonDown
    WinAPIShapes::hReg(WM_RBUTTONDOWN, &WinAPIShapes::onRButtonDown);

    // onMouseMove
    WinAPIShapes::hReg(WM_MOUSEMOVE, &WinAPIShapes::onMouseMove);

    // onMouseWheel
    WinAPIShapes::hReg(WM_MOUSEWHEEL, &WinAPIShapes::onMouseWheel);

    // onCommand
    WinAPIShapes::hReg(WM_COMMAND, &WinAPIShapes::onCommand);

    // onChar
    WinAPIShapes::hReg(WM_CHAR, &WinAPIShapes::onChar);

    // onTimer
    WinAPIShapes::hReg(WM_TIMER, &WinAPIShapes::onTimer);

    // onFileChange
    WinAPIShapes::hReg(ID_FILECHANGE, &WinAPIShapes::onFileChange);

    // onFileOpen
    WinAPIShapes::hReg(IDC_FILEOPEN, &WinAPIShapes::onFileOpen);

    // onFileOpen
    WinAPIShapes::hReg(IDC_FILESAVE, &WinAPIShapes::onFileSave);

    // onCalcSize
    WinAPIShapes::hReg(WM_NCCALCSIZE, &WinAPIShapes::onNCCalcSize);
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
    return DefWindowProc(this->m_hWnd, message, wParam, lParam);
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

    ::EndBufferedPaint(hbuf, true);
    EndPaint(this->m_hWnd, &ps);
    ReleaseDC(this->m_hWnd, memDc); // check if this is correct, might not be
    ReleaseDC(this->m_hWnd, hdc);
}

void WinAPIShapes::onCreate(WPARAM wParam, LPARAM lParam) {
    RECT rect;
    // GetWindowRect(); // includes title bar, menu bar, etc.
    GetClientRect(this->m_hWnd, &rect);

    this->m_lbhWnd = CreateWindow(
        WC_LISTBOX, 
        TEXT("Mouse Events"),
        WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY | LBS_HASSTRINGS,
        10, 
        rect.bottom - 10 - 100, 
        rect.right - 20 - 100 - 10, 
        100,
        this->m_hWnd, 
        NULL, 
        (HINSTANCE)GetWindowLongPtr(this->m_hWnd, GWLP_HINSTANCE), 
        NULL);

    this->loadFile();

    // find the bounds of the window
    this->m_height = rect.bottom - 120;
    this->m_width = rect.right;

    // create the timer button
    this->m_timerBtn = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"START",   // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        rect.right - 100 - 10,  // x position 
        rect.bottom - 100 - 10, // y position 
        100,        // Button width
        100,        // Button height
        this->m_hWnd,   // Parent window
        (HMENU)IDC_TIMER,       // No menu.
        (HINSTANCE)GetWindowLongPtr(this->m_hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
    this->m_timerOn = false;
}

void WinAPIShapes::onDestroy(WPARAM wParam, LPARAM lParam) {
    this->saveFile();
    PostQuitMessage(0);
}

void WinAPIShapes::onLButtonDown(WPARAM wParam, LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // check if the mouse is on a shape
    if (isOnShape(LOWORD(lParam), HIWORD(lParam))) {
        bool onShape = false;
        Square s(x, y, Shape::defaultWidth); // square hitbox

        for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
            if ((*it)->overlap(s)) {  // check for mouse on shape
                if (this->m_selected.empty() || MK_CONTROL == (MK_CONTROL & wParam)) { // if there are no selected shapes
                    if (!(*it)->getSelected()) {    // select if the shape is not selected
                        selectedShape sl;
                        sl.x = (*it)->getX();
                        sl.y = (*it)->getY();
                        sl.s = (*it);
                        this->m_selected.push_back(sl);
                        (*it)->setSelected(true);
                    }
                    else {                          // deselect if the shape is already selected
                        for (auto jt = this->m_selected.begin(); jt != this->m_selected.end(); ++jt) {
                            if (*it == jt->s) {
                                jt->s->setSelected(false);
                                for (auto itr = this->m_shapes.begin(); itr != this->m_shapes.end(); ++itr) {
                                    if ((*itr)->overlap(*jt->s)) {
                                        jt->s->setX(jt->x);
                                        jt->s->setY(jt->y);
                                    }
                                }
                                this->m_selected.erase(jt);
                                break;
                            }
                        }
                    }
                }
                else {  // deselect all and select the shape currently being clicked
                    // add the current shape
                    bool notSelected = false;
                    if (!(*it)->getSelected()) {
                        notSelected = true;
                    }
                        
                    // deselect all
                    for (auto jt = this->m_selected.begin(); jt != this->m_selected.end(); ++jt) {
                        jt->s->setSelected(false);
                        for (auto itr = this->m_shapes.begin(); itr != this->m_shapes.end(); ++itr) {
                            if ((*itr)->overlap(*jt->s)) {
                                jt->s->setX(jt->x);
                                jt->s->setY(jt->y);
                            }
                        }
                    }
                    this->m_selected.clear();

                    
                    // add the current shape as selected
                    if (notSelected) {
                        selectedShape sl;
                        sl.x = (*it)->getX();
                        sl.y = (*it)->getY();
                        sl.s = (*it);
                        this->m_selected.push_back(sl);
                        (*it)->setSelected(true);
                    }
                }
                // cannot click on more than one shape at once
                RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
                break;
            }
        }
    }
    else if (this->m_selected.empty())
        // create a circle
        this->createShape<Circle>(x, y);

    this->m_mousedown = true;
    this->m_movedX = x;
    this->m_movedY = y;
}

void WinAPIShapes::onLButtonUp(WPARAM wParam, LPARAM lParam) {
    this->m_mousedown = false;
    this->m_movedX = 0;
    this->m_movedY = 0;
}

void WinAPIShapes::onRButtonDown(WPARAM wParam, LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // check if the mouse is on a shape
    if (!isOnShape(LOWORD(lParam), HIWORD(lParam)))
        // create a square
        this->createShape<Square>(x, y);
}

void WinAPIShapes::onMouseMove(WPARAM wParam, LPARAM lParam) {
    if (this->m_mousedown) {
        int x, y;
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        for (auto it = this->m_selected.begin(); it != this->m_selected.end(); ++it)
            this->moveShape(lParam, it->s, (x - this->m_movedX), (y - this->m_movedY));
        this->m_movedX = x;
        this->m_movedY = y;
    }
}

void WinAPIShapes::onMouseWheel(WPARAM wParam, LPARAM lParam) {
    if (!this->m_selected.empty())
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
    {
        DestroyWindow(this->m_hWnd);
        break;
    }
    case IDC_TIMER:
    {
        // check for the right message
        switch (HIWORD(wParam)) {
        case BN_CLICKED:
        {
            RECT rect;
            GetClientRect(this->m_hWnd, &rect);

            // WM_SETTEXT
            // NOTE: do not store information in graphical interfaces
            if (this->m_timerOn) {
                KillTimer(this->m_hWnd, 1);
                ::SetWindowText(this->m_timerBtn, TEXT("START"));
            }
            else {
                // set the timer
                SetTimer(this->m_hWnd, 1, 20, nullptr);
                ::SetWindowText(this->m_timerBtn, TEXT("STOP"));
            }
            this->m_timerOn = !this->m_timerOn;
        }
        }
    }
    }
}

void WinAPIShapes::onChar(WPARAM wParam, LPARAM lParam) {
    switch (wParam) {
    case 0x08:
        // case of backspace being clicked
        WinAPIShapes::onDelete();
    case VK_ESCAPE:
        // case of escape being clicked
        WinAPIShapes::onEscape();
    }
}

void WinAPIShapes::onDelete() {
    if (!this->m_selected.empty()) {
        for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
            for (auto jt = this->m_selected.begin(); jt != this->m_selected.end(); ++jt) {
                if (*it == jt->s) {
                    RECT deletedArea{ jt->s->getX() - (jt->s->getWidth() / 2) - 6, jt->s->getY() - (jt->s->getWidth() / 2) - 6,
                        jt->s->getX() + (jt->s->getWidth() / 2) + 6, jt->s->getY() + (jt->s->getWidth() / 2) + 6 };
                    delete* it;
                    this->m_shapes.erase(it);
                    this->m_selected.erase(jt);
                    InvalidateRect(this->m_hWnd, &deletedArea, true); // redraws the area of the circle deleted
                    return;
                }
            }
        }
    }
}

void WinAPIShapes::onEscape() {
    for (auto it = this->m_selected.begin(); it != this->m_selected.end(); ++it)
        it->s->setSelected(false);
    this->m_selected.clear();
    RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void WinAPIShapes::onTimer(WPARAM wParam, LPARAM lParam) {
    int speed = 5;
    // if updating position of shape "it" results in colision, don't move in the position and search for another safe position
    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); ++it) {
        if (!(*it)->getSelected()) {
            for (auto jt = it; jt != this->m_shapes.end(); ++jt) {
                Square* a = new Square((*it)->getX(), (*it)->getY(), (*it)->getWidth(), (*it)->getXVel(), (*it)->getYVel());
                Square* b = new Square((*jt)->getX(), (*jt)->getY(), (*jt)->getWidth(), (*jt)->getXVel(), (*jt)->getYVel());
                a->move(speed);
                if (!(*jt)->getSelected())
                    b->move(speed);

                if (a->overlap(*b) && *it != *jt) {
                    // include speed considerations later
                    //if ((a->getX() + (a->getWidth() / 2)) >= (b->getX() - (b->getWidth() / 2))) {
                    //    (*it)->setVel(-1, 1);
                    //    (*jt)->setVel(-1, 1);
                    //}
                    //else if ((a->getX() - (a->getWidth() / 2)) >= (b->getX() + (b->getWidth() / 2))) {
                    //    (*it)->setVel(-1, 1);
                    //    (*jt)->setVel(-1, 1);
                    //}
                    //if ((a->getY() + (a->getWidth() / 2)) >= (b->getY() - (b->getWidth() / 2))) {
                    //    (*it)->setVel(1, -1);
                    //    (*jt)->setVel(1, -1);
                    //}
                    //else if ((a->getY() - (a->getWidth() / 2)) >= (b->getY() + (b->getWidth() / 2))) {
                    //    (*it)->setVel(1, -1);
                    //    (*jt)->setVel(1, -1);
                    if ((*it)->getX() + ((*it)->getWidth()/2) + (speed * (*it)->getXVel()) > (*jt)->getX() - ((*jt)->getWidth()/2) &&
                        (*it)->getX() - ((*it)->getWidth() / 2) + (speed * (*it)->getXVel()) < (*jt)->getX() + ((*jt)->getWidth() / 2) &&
                        (*it)->getY() + ((*it)->getWidth() / 2) > (*jt)->getY() - ((*jt)->getWidth() / 2) &&
                        (*it)->getY() - ((*it)->getWidth() / 2) < (*jt)->getY() + ((*jt)->getWidth() / 2)) {
                        (*it)->setVel(-1, 1);
                        (*jt)->setVel(-1, 1);
                    }
                    if ((*it)->getX() + ((*it)->getWidth() / 2) > (*jt)->getX() - ((*jt)->getWidth() / 2) &&
                        (*it)->getX() - ((*it)->getWidth() / 2) < (*jt)->getX() + ((*jt)->getWidth() / 2) &&
                        (*it)->getY() + ((*it)->getWidth() / 2) + (speed * (*it)->getYVel()) > (*jt)->getY() - ((*jt)->getWidth() / 2) &&
                        (*it)->getY() - ((*it)->getWidth() / 2) + (speed * (*it)->getYVel()) < (*jt)->getY() + ((*jt)->getWidth() / 2)) {
                        (*it)->setVel(1, -1);
                        (*jt)->setVel(1, -1);
                    }
                }
                delete a;
                delete b;
            }
            this->collide(*it);
            (*it)->move(speed);
        }
    }

    // move each shape


    RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void WinAPIShapes::onFileChange(WPARAM wParam, LPARAM lParam) {
    this->m_filename = LPCTSTR(lParam);
}

void WinAPIShapes::onFileOpen(WPARAM wParam, LPARAM lParam) {
    this->loadFile();
    RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void WinAPIShapes::onFileSave(WPARAM wParam, LPARAM lParam) {
    this->saveFile();
}

// TODO: ask about this in class
void WinAPIShapes::onNCCalcSize(WPARAM wParam, LPARAM lParam) {
    //LPNCCALCSIZE_PARAMS pncc = (LPNCCALCSIZE_PARAMS)lParam;
    //::DefWindowProc(this->m_hWnd, WM_NCCALCSIZE, wParam, lParam);
    //pncc->rgrc[0].right -= 200;
}

void WinAPIShapes::loadFile() {
    std::wifstream ifile;
    ifile.open(this->m_filename, ios::in);

    // clear previous data
    this->m_shapes.clear();
    Shape::resetCnt();

    // read from the file
    if (ifile.is_open()) {
        int size;
        ifile >> size;
        ifile.ignore(1);

        for (int i = 0; i < size; i++) {
            char iden = ifile.peek();
            Shape* s = shapeFactory(iden);

            s->load(ifile);

            this->m_shapes.push_back(s);
        }
    }

    ifile.close();
}

void WinAPIShapes::saveFile() {
    // open a file
    std::wofstream wfile;
    wfile.open(this->m_filename, ios::out);

    // edit the file
    if (wfile.is_open()) {
        wfile << this->m_shapes.size() << endl;
        for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
            (*it)->save(wfile);
        }
    }

    // close the file
    wfile.close();
}

void WinAPIShapes::moveShape(LPARAM lParam, Shape* s, int x, int y) {
    // save old locations (for drawing purposes only)
    int old_x = s->getX();
    int old_y = s->getY();

    s->setPos(s->getX() + x, s->getY() + y);

    // redraw the window
    RECT newArea{ x - (s->getWidth() / 2) - 6, y - (s->getWidth() / 2) - 6,
        x + (s->getWidth() / 2) + 6, y + (s->getWidth() / 2) + 6 }; // gets the attributes one by one
    RECT oldArea{ old_x - (s->getWidth() / 2) - 6, old_y - (s->getWidth() / 2) - 6,
        old_x + (s->getWidth() / 2) + 6, old_y + (s->getWidth() / 2) + 6 };
    RECT* totalArea = this->fusedRect(newArea, oldArea);
    InvalidateRect(this->m_hWnd, totalArea, false);
    delete totalArea;
}

void WinAPIShapes::releaseShape(LPARAM lParam) {
    int x, y;
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    for (auto it = this->m_selected.begin(); it != this->m_selected.end(); ++it) {
        TCHAR buffer[128]{};
        StringCchPrintf(buffer, 128, TEXT("Shape wants to move from x = [%d], y = [%d] to x = [%d], y = [%d]"), it->s->getX(), it->s->getY(), x, y);
        SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)buffer);

        // check for overlapping
        bool overlap = false;
        for (auto jt = this->m_shapes.begin(); jt != this->m_shapes.end() && !overlap; jt++) {
            overlap = (*jt)->overlap(*it->s);
        }

        // if there's no overlap, place the shape
        if (!overlap) {
            it->s->setPos(x, y);
            SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Shape moved successfully!"));
        }

        // if there is overlap, return shape to original position
        else {
            it->s->setPos(it->x, it->y);
            SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)TEXT("Shape overlapping with a preexisting shape! Not moved."));

            // redraw area with circle
            RECT newArea{ it->s->getX() - (it->s->getWidth() / 2) - 6, it->s->getY() - (it->s->getWidth() / 2) - 6,
                it->s->getX() + (it->s->getWidth() / 2) + 6, it->s->getY() + (it->s->getWidth() / 2) + 6 };
            InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
        }

        // redraw area with circle
        RECT newArea{ x - (it->s->getWidth() / 2) - 6, y - (it->s->getWidth() / 2) - 6,
            x + (it->s->getWidth() / 2) + 6, y + (it->s->getWidth() / 2) + 6 };
        InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen

        it->s->setSelected(false);
        this->m_selected.erase(it);
    }
}

void WinAPIShapes::resizeShape(LPARAM lParam, WPARAM wParam) {
    int scrollQuant = HIWORD(wParam);
    int sizeChange;

    for (auto it = this->m_selected.begin(); it != this->m_selected.end(); ++it) {
        if (scrollQuant == 120) // scroll away
            sizeChange = 4;
        else if (it->s->getWidth() > 32)
            sizeChange = -4;
        else
            sizeChange = 0;

        // get old width
        int oldWidth = it->s->getWidth();

        // change width
        it->s->setWidth(it->s->getWidth() + sizeChange);

        int x, y;
        x = LOWORD(lParam);
        y = HIWORD(lParam);

        // keeps x and y in the center
        x -= sizeChange / 2;
        y += sizeChange / 2;

        if (sizeChange > 0) {
            // redraw area with circle
            RECT newArea{ it->s->getX() - (it->s->getWidth() / 2) - 3, it->s->getY() - (it->s->getWidth() / 2) - 3, it->s->getX() + (it->s->getWidth() / 2) + 3, it->s->getY() + (it->s->getWidth() / 2) + 3 };
            InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
        }
        else {
            // redraw area with circle
            RECT newArea{ it->s->getX() - (oldWidth / 2) - 3, it->s->getY() - (oldWidth / 2) - 3, it->s->getX() + (oldWidth / 2) + 3, it->s->getY() + (oldWidth / 2) + 3 };
            InvalidateRect(this->m_hWnd, &newArea, true); // only modifies a certain rectangle of the screen
        }
    }
}

bool WinAPIShapes::isOnShape(int x, int y) {
    bool onShape = false;
    Square s(x, y, Shape::defaultWidth); // square hitbox
    Shape* selected = this->select();

    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
        if ((*it)->overlap(s)) {
            onShape = true;
            break;
        }
    }

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

// manages border collisions
void WinAPIShapes::collide(Shape* s) {
    // border collisions
    if (s->getX() + (s->getWidth() / 2) > this->m_width) {
        // change trajectory
        s->setVel(-1, 1);
        s->setX(this->m_width - (s->getWidth() / 2));
    }
    else if (s->getX() - (s->getWidth() / 2) < 0) {
        s->setVel(-1, 1);
        s->setX(s->getWidth() / 2);
    }
    if (s->getY() + (s->getWidth() / 2) > this->m_height) {
        s->setVel(1, -1);
        s->setY(this->m_height - (s->getWidth() / 2));
    }
    else if (s->getY() - (s->getWidth() / 2) < 0) {
        s->setVel(1, -1);
        s->setY(s->getWidth() / 2);
    }
}

Shape* WinAPIShapes::select() {
    Shape* s = nullptr;
    for (auto it = this->m_shapes.begin(); it != this->m_shapes.end(); it++) {
        if ((*it)->getSelected())
            s = *it;
    }
    return s;
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
