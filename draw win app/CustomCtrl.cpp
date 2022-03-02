#include <Windows.h>
#include <gdiplus.h>
#include <Uxtheme.h>
#include "CustomCtrl.h"
#include "WinAPIShapes.h"
#include "Shape.h"

using namespace Gdiplus;

void RegisterControl(HINSTANCE hInstance) {
    WNDCLASSEX wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);
    //wcex.style = WS_CHILD | WS_VISIBLE;
    wcex.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = CustomCtrl;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(220, 220, 220));
    wcex.lpszClassName = TEXT("MyCustomCtrl");

    auto regRes = RegisterClassEx(&wcex);

    //if (regRes == 0) {
    //    auto result = GetLastError();
    //    MessageBox(0, TEXT("The Window Class for the custom constrol could not be registered."), TEXT("Window Class Registration"), MB_OK | MB_ICONERROR);
    //}
    //else
    //    MessageBox(0, TEXT("Success registering the Window Class."), TEXT("Window Class Registration"), MB_OK | MB_ICONINFORMATION);
}

HWND CreateControl(HWND hWnd) {
    // Create a custom control in the main window
    return CreateWindow(
        TEXT("MyCustomCtrl"),
        TEXT("Control Text"),
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        20,
        100,
        200,
        50,
        hWnd,
        nullptr,
        GetModuleHandle(NULL),
        nullptr
    );
}

// Custom Ctrl WndProc
LRESULT CALLBACK CustomCtrl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool isClick = false;
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;
        RECT rect;

        GetClientRect(hWnd, &rect); // gets the custom control dimensions
        hdc = BeginPaint(hWnd, &ps);
        SetBkMode(hdc, TRANSPARENT);

        //SetTextColor(hdc, RGB(255, 87, 34));
        //if (isClick)
        //    DrawText(hdc, TEXT("Click"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        //else
        //    DrawText(hdc, TEXT("Custom Control"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        HDC memDc;
        RECT rc;
        auto hbuf = ::BeginBufferedPaint(hdc, &rc, BPBF_COMPATIBLEBITMAP, nullptr, &memDc);

        // if hbuf is not 0, do double buffering, otherwise do single buffering
        Graphics graphics(memDc);

        // calculate dimensions for coloured square
        int width = rect.bottom - rect.top - 6; 
        RECT sqr;
        sqr.top = rect.top + 3;
        sqr.left = rect.left + 3;
        sqr.bottom = sqr.top + width;
        sqr.right = sqr.left + width;
        HBRUSH brush = CreateSolidBrush(RGB(Shape::defaultBorderColour.GetRed(), Shape::defaultBorderColour.GetGreen(), Shape::defaultBorderColour.GetBlue()));
        FillRect(hdc, &sqr, brush);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        CHOOSECOLOR cc;                 // common dialog box structure
        static COLORREF acrCustClr[16]; // array of custom color

        // initialize CHOOSECOLOR
        ZeroMemory(&cc, sizeof(cc));
        cc.lStructSize = sizeof(cc);
        cc.hwndOwner = hWnd;
        cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = (DWORD)RGB(Shape::defaultBorderColour.GetRed(), Shape::defaultBorderColour.GetGreen(), Shape::defaultBorderColour.GetBlue());
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColor(&cc) == TRUE) {
            Shape::defaultBorderColour.SetFromCOLORREF(cc.rgbResult);
            // GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult)
        }
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        isClick = false;
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}