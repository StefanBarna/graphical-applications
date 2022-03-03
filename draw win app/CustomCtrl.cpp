#include <Windows.h>
#include <gdiplus.h>
#include <Uxtheme.h>
#include <string.h>
#include <strsafe.h>
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
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);

        // draw in a coloured square
        int width = rect.bottom - rect.top - 8;
        RectF sqr(rect.top + 4, rect.left + 4, width, width);
        SolidBrush brush(Shape::defaultBorderColour);
        graphics.FillRectangle(&brush, sqr);

        // print text
        Font myFont(L"Arial", 8);
        SolidBrush textBrush(Color::Black);
        TCHAR hexVal[8];
        StringCchPrintf(hexVal, 8, TEXT("#%02X%02X%02X"), Shape::defaultBorderColour.GetR(), Shape::defaultBorderColour.GetG(), Shape::defaultBorderColour.GetB());
        PointF textOrigin(sqr.GetRight() + 3, rect.top + 5);
        graphics.DrawString(hexVal, 7, &myFont, textOrigin, &textBrush);

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