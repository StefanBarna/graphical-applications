#include <sstream>
#include <Windows.h>
#include <gdiplus.h>

#include "ColourCtrl.h"
#include "WinAPIShapes.h"

using namespace Gdiplus;

ColourCtrl::MessageMap ColourCtrl::ms_msgMap[100]{};
size_t ColourCtrl::ms_cnt{};

ColourCtrl* ColourCtrl::ms_pWnd[100]{};
size_t ColourCtrl::ms_wndCnt{};

void ColourCtrl::staticConstructor() {
	// onCreate
	ColourCtrl::hReg(WM_CREATE, &ColourCtrl::onCreate);

	// onGetDlgCode
	ColourCtrl::hReg(WM_GETDLGCODE, &ColourCtrl::onGetDlgCode);

	// onChar
	ColourCtrl::hReg(WM_CHAR, &ColourCtrl::onChar);

	// onPaint
	ColourCtrl::hReg(WM_PAINT, &ColourCtrl::onPaint);

	// onLButtonDown
	ColourCtrl::hReg(WM_LBUTTONDOWN, &ColourCtrl::onLButtonDown);
}

void ColourCtrl::hReg(UINT message, MessageHandler hMsg) {
	ColourCtrl::ms_msgMap[ColourCtrl::ms_cnt].msg = message;
	ColourCtrl::ms_msgMap[ColourCtrl::ms_cnt].pfnHandler = hMsg;
	ColourCtrl::ms_cnt++;
}

ColourCtrl::ColourCtrl() {
	if (ColourCtrl::ms_cnt == 0) {
		// register the class if this is the first time it is called
		WNDCLASSEX wcex{};

		wcex.cbSize = sizeof(WNDCLASSEX);
		//wcex.style = WS_CHILD | WS_VISIBLE;
		wcex.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = ColourCtrl::WndProcClass;
		wcex.hInstance = WinAPIShapes::ms_hInstance;
		wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
		wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(220, 220, 220));
		wcex.lpszClassName = TEXT("MyCustomCtrl");

		auto regRes = RegisterClassEx(&wcex);

		ColourCtrl::staticConstructor();
	}
}

ColourCtrl* ColourCtrl::findWindow(HWND hWnd) {
	for (size_t i = 0; i < ColourCtrl::ms_wndCnt; ++i) {
		if (ColourCtrl::ms_pWnd[i]->m_hWnd == hWnd)
			return ColourCtrl::ms_pWnd[i];
	}
	return nullptr;
}

LRESULT CALLBACK ColourCtrl::WndProcClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	ColourCtrl* wnd = ColourCtrl::findWindow(hWnd);
	
	if (wnd != nullptr)
		return wnd->WndProc(message, wParam, lParam);

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ColourCtrl::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	for (size_t i = 0; i < ColourCtrl::ms_cnt; ++i) {
		if (message == ColourCtrl::ms_msgMap[i].msg)
			return (this->*ColourCtrl::ms_msgMap[i].pfnHandler)(wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

BOOL ColourCtrl::create(HWND parent) {
	ColourCtrl::ms_pWnd[ColourCtrl::ms_wndCnt] = this;
	++ColourCtrl::ms_wndCnt;

	// Create a custom control in the main window
	this->m_hWnd = CreateWindow(
		TEXT("MyCustomCtrl"),
		TEXT("Control Text"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		20,
		100,
		200,
		50,
		parent,
		nullptr,
		GetModuleHandle(NULL),
		nullptr
	);

	if (!this->m_hWnd)
		return FALSE;
	return TRUE;
}

INT_PTR ColourCtrl::onCreate(WPARAM wParam, LPARAM lParam) {
	auto styles = GetWindowLong(this->m_hWnd, GWL_STYLE);
	if (styles & WS_TABSTOP)
	    styles = 0;
	
	colourCode.clear();
	wchar_t buffer[128];
	std::swprintf(buffer, sizeof(buffer), L"#%02X%02X%02X", Shape::defaultBorderColour.GetR(), Shape::defaultBorderColour.GetG(), Shape::defaultBorderColour.GetB());
	colourCode.append(buffer);
	return 0;
}

INT_PTR ColourCtrl::onGetDlgCode(WPARAM wParam, LPARAM lParam) {
	return DLGC_WANTMESSAGE;
}

INT_PTR ColourCtrl::onChar(WPARAM wParam, LPARAM lParam) {
	// if the code index was complete, start a new code
    if (colourCode.size() == 7) {
        colourCode.clear();
        colourCode += '#';
        /*codeIdx = 0;*/
    }
    // if an acceptable character, add it to the colour code
    if ((48 <= (char)wParam && (char)wParam <= 57)
        || (65 <= (char)wParam && (char)wParam <= 70)
        || (97 <= (char)wParam && (char)wParam <= 102)) {
        //colourCode[codeIdx] = (char)wParam;
        colourCode += (char)wParam;
        //codeIdx++;
    }
    // if after adding the character the colour code is complete, push it
    if (colourCode.size() == 7) {
        int colourVal[3];
        for (int i = 0; i < 3; i++) {
            std::stringstream ss;
            std::string colourHex;
            colourHex += colourCode[i * 2 + 1];
            colourHex += colourCode[i * 2 + 2];
            ss << colourHex;
            ss >> std::hex >> colourVal[i];
        }

        Shape::defaultBorderColour.SetFromCOLORREF(RGB(colourVal[0], colourVal[1], colourVal[2]));
    }

    RedrawWindow(this->m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
    return 0;
}

INT_PTR ColourCtrl::onPaint(WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;

    GetClientRect(this->m_hWnd, &rect); // gets the custom control dimensions
    hdc = BeginPaint(this->m_hWnd, &ps);
    SetBkMode(hdc, OPAQUE);
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
    //TCHAR hexVal[8];
    //StringCchPrintf(hexVal, 8, TEXT("#%02X%02X%02X"), Shape::defaultBorderColour.GetR(), Shape::defaultBorderColour.GetG(), Shape::defaultBorderColour.GetB());
    PointF textOrigin(sqr.GetRight() + 3, rect.top + 5);
    graphics.DrawString(colourCode.c_str(), colourCode.size(), &myFont, textOrigin, &textBrush);

    EndPaint(this->m_hWnd, &ps);
    return 0;
}

INT_PTR ColourCtrl::onLButtonDown(WPARAM wParam, LPARAM lParam) {
    SetFocus(this->m_hWnd);
    return 0;
}