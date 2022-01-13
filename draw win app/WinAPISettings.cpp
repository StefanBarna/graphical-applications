#include <Windows.h>
#include <gdiplus.h>
#include <Commctrl.h>

#include "WinAPISettings.h"
#include "WinAPIShapes.h"
#include "Shape.h"
#include "resource.h"

// TODO for next class
// - find out more about the shader files
// - open/save file dialog
// - animation in DirectX
// - custom 
// - DirectX antialiasing

WinAPISettings::MessageMap WinAPISettings::ms_msgMap[100]{};
size_t WinAPISettings::ms_cnt{};

WinAPISettings* WinAPISettings::ms_pWnd[100]{};
size_t WinAPISettings::ms_wndCnt{};

void WinAPISettings::staticConstructor() {
    // onInitDialog
    WinAPISettings::hReg(WM_INITDIALOG, &WinAPISettings::onInitDialog);

	// onCommand
    WinAPISettings::hReg(WM_COMMAND, &WinAPISettings::onCommand);
}

void WinAPISettings::hReg(UINT message, MessageHandler hMsg) {
    WinAPISettings::ms_msgMap[WinAPISettings::ms_cnt].msg = message;
    WinAPISettings::ms_msgMap[WinAPISettings::ms_cnt].pfnHandler = hMsg;
    WinAPISettings::ms_cnt++;
}

WinAPISettings::WinAPISettings() {
    if (WinAPISettings::ms_cnt == 0)
        WinAPISettings::staticConstructor();
}

WinAPISettings* WinAPISettings::findWindow(HWND hDlg) {
    for (size_t i = 0; i < WinAPISettings::ms_wndCnt; ++i) {
        if (WinAPISettings::ms_pWnd[i]->m_hDlg == hDlg)
            return WinAPISettings::ms_pWnd[i];
    }
    return nullptr;
}

INT_PTR CALLBACK WinAPISettings::WndProcClass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    WinAPISettings* dlg = WinAPISettings::findWindow(hWnd);

    if (dlg == nullptr) {
        if (message == WM_INITDIALOG) {
            // convert lparam to winapisettings
            int a{};
            dlg = (WinAPISettings*)lParam;
            dlg->m_hDlg = hWnd;
            // add preexisting values for RGB and width
        }
    }
    if (dlg != nullptr)
        return dlg->WndProc(message, wParam, lParam);

    // a nice way to discard messages
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WinAPISettings::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	for (size_t i = 0; i < WinAPISettings::ms_cnt; ++i) {
		if (message == WinAPISettings::ms_msgMap[i].msg)
			return (this->*WinAPISettings::ms_msgMap[i].pfnHandler)(wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

INT_PTR WinAPISettings::onInitDialog(WPARAM wParam, LPARAM lParam) {
    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", Shape::defaultWidth);
    ::SetDlgItemText(this->m_hDlg, IDC_WIDTH_INPUT, buffer);
    wsprintfW(buffer, L"%d", (int)(Shape::defaultBorderColour.GetRed()));
    ::SetDlgItemText(this->m_hDlg, IDC_RED_INPUT, buffer);
    wsprintfW(buffer, L"%d", (int)(Shape::defaultBorderColour.GetGreen()));
    ::SetDlgItemText(this->m_hDlg, IDC_GREEN_INPUT, buffer);
    wsprintfW(buffer, L"%d", (int)(Shape::defaultBorderColour.GetBlue()));
    ::SetDlgItemText(this->m_hDlg, IDC_BLUE_INPUT, buffer);


    SendMessage(this->m_hDlg, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 20));
    return (INT_PTR)TRUE;
}

INT_PTR WinAPISettings::onCommand(WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
    case IDOK: {
        this->onOk(wParam, lParam);
    }
    case IDCANCEL: {
        ::EndDialog(this->m_hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
    } break;
    case IDC_COLOURCHOOSE: {
        this->onColour(wParam, lParam);
    } break;
    }
    return (INT_PTR)FALSE;
}

void WinAPISettings::onOk(WPARAM wParam, LPARAM lParam) {
    auto parentWnd = ::GetParent(this->m_hDlg);

    // receive information from text areas
    TCHAR* widthBuffer = new TCHAR[128]{};
    ::GetDlgItemText(this->m_hDlg, IDC_WIDTH_INPUT, widthBuffer, 128);

    // convert it to readable information
    if (widthBuffer[0] != '\0') {
        int width = _wtoi(widthBuffer);
        Shape::defaultWidth = width;
    }

    // delete tchar strings
    delete[] widthBuffer;
}

void WinAPISettings::onColour(WPARAM wParam, LPARAM lParam) {
    CHOOSECOLOR cc;                 // common dialog box structure
    static COLORREF acrCustClr[16]; // array of custom color

    // initialize CHOOSECOLOR
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = this->m_hDlg;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = (DWORD)RGB(Shape::defaultBorderColour.GetRed(), Shape::defaultBorderColour.GetGreen(), Shape::defaultBorderColour.GetBlue());
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc) == TRUE) {
        Shape::defaultBorderColour.SetFromCOLORREF(cc.rgbResult);
        // GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult)
    }
}

BOOL WinAPISettings::create(HWND parent) {
    WinAPISettings::ms_pWnd[WinAPISettings::ms_wndCnt] = this;
    ++WinAPISettings::ms_wndCnt;

    DialogBoxParam(WinAPIShapes::ms_hInstance, MAKEINTRESOURCE(IDD_SETTINGS), parent, WinAPISettings::WndProcClass, (LPARAM)this);

    return TRUE;
}