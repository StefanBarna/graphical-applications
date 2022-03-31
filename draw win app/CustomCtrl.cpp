//#include <Windows.h>
//#include <gdiplus.h>
//#include <Uxtheme.h>
//#include <string>
//#include <format>
//#include <sstream>
//#include <strsafe.h>
//#include "ColourCtrl.h"
//#include "WinAPIShapes.h"
//#include "Shape.h"
//
//// TODO: static cursor that displays where your keyboard is
//// TODO: create a round button for play
//// TODO: create a custom control managing two buttons that manages the behaviours
//
//// TODO: colour control class
//
//using namespace Gdiplus;
//
//std::wstring colourCode;
//
//void RegisterControl(HINSTANCE hInstance) {
//    WNDCLASSEX wcex{};
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//    //wcex.style = WS_CHILD | WS_VISIBLE;
//    wcex.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc = ColourCtrl;
//    wcex.hInstance = hInstance;
//    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
//    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(220, 220, 220));
//    wcex.lpszClassName = TEXT("MyCustomCtrl");
//
//    auto regRes = RegisterClassEx(&wcex);
//}
//
//HWND CreateControl(HWND hWnd) {
//    // Create a custom control in the main window
//    return CreateWindow(
//        TEXT("MyCustomCtrl"),
//        TEXT("Control Text"),
//        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
//        20,
//        100,
//        200,
//        50,
//        hWnd,
//        nullptr,
//        GetModuleHandle(NULL),
//        nullptr
//    );
//}
//
//// Custom Ctrl WndProc
//LRESULT CALLBACK ColourCtrl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_CREATE:
//    {
//        auto styles = GetWindowLong(hWnd, GWL_STYLE);
//        if (styles & WS_TABSTOP)
//            styles = 0;
//
//        colourCode.clear();
//        wchar_t buffer[128];
//        std::swprintf(buffer, sizeof(buffer), L"#%02X%02X%02X", Shape::defaultBorderColour.GetR(), Shape::defaultBorderColour.GetG(), Shape::defaultBorderColour.GetB());
//        colourCode.append(buffer);
//        //colourCode.size() = 6;
//        return 0;
//    }
//    case WM_GETDLGCODE:
//    {
//        //if (lParam != NULL) {
//        //    MSG* messageStruct = (MSG*)lParam;
//        //    if (messageStruct->message == WM_CHAR)
//        //        MessageBox(hWnd, TEXT("correct message!"), TEXT("message notif"), MB_OK);
//        //}
//        return DLGC_WANTMESSAGE;
//    }
//    case WM_CHAR:
//    {
//        // if the code index was complete, start a new code
//        if (colourCode.size() == 7) {
//            colourCode.clear();
//            colourCode += '#';
//            /*codeIdx = 0;*/
//        }
//        // if an acceptable character, add it to the colour code
//        if ((48 <= (char)wParam && (char)wParam <= 57)
//            || (65 <= (char)wParam && (char)wParam <= 70)
//            || (97 <= (char)wParam && (char)wParam <= 102)) {
//            //colourCode[codeIdx] = (char)wParam;
//            colourCode += (char)wParam;
//            //codeIdx++;
//        }
//        // if after adding the character the colour code is complete, push it
//        if (colourCode.size() == 7) {
//            int colourVal[3];
//            for (int i = 0; i < 3; i++) {
//                std::stringstream ss;
//                std::string colourHex;
//                colourHex += colourCode[i * 2 + 1];
//                colourHex += colourCode[i * 2 + 2];
//                ss << colourHex;
//                ss >> std::hex >> colourVal[i];
//            }
//
//            Shape::defaultBorderColour.SetFromCOLORREF(RGB(colourVal[0], colourVal[1], colourVal[2]));
//        }
//
//        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
//        return 0;
//    }
//    case WM_PAINT:
//    {
//        PAINTSTRUCT ps;
//        HDC hdc;
//        RECT rect;
//
//        GetClientRect(hWnd, &rect); // gets the custom control dimensions
//        hdc = BeginPaint(hWnd, &ps);
//        SetBkMode(hdc, OPAQUE);
//        Gdiplus::Graphics graphics(hdc);
//        graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
//
//        // draw in a coloured square
//        int width = rect.bottom - rect.top - 8;
//        RectF sqr(rect.top + 4, rect.left + 4, width, width);
//        SolidBrush brush(Shape::defaultBorderColour);
//        graphics.FillRectangle(&brush, sqr);
//
//        // print text
//        Font myFont(L"Arial", 8);
//        SolidBrush textBrush(Color::Black);
//        //TCHAR hexVal[8];
//        //StringCchPrintf(hexVal, 8, TEXT("#%02X%02X%02X"), Shape::defaultBorderColour.GetR(), Shape::defaultBorderColour.GetG(), Shape::defaultBorderColour.GetB());
//        PointF textOrigin(sqr.GetRight() + 3, rect.top + 5);
//        graphics.DrawString(colourCode.c_str(), colourCode.size(), &myFont, textOrigin, &textBrush);
//
//        EndPaint(hWnd, &ps);
//        return 0;
//    }
//    case WM_LBUTTONDOWN:
//    {
//        SetFocus(hWnd);
//        return 0;
//    }
//    }
//    return DefWindowProc(hWnd, message, wParam, lParam);
//}