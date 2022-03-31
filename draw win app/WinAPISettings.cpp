#include <Windows.h>
#include <gdiplus.h>
#include <Commctrl.h>
#include <strsafe.h>

#include "WinAPISettings.h"
#include "WinAPIShapes.h"
#include "Shape.h"
#include "resource.h"
#include "ColourCtrl.h"

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
            dlg = (WinAPISettings*)lParam;
            dlg->m_hDlg = hWnd;
        }

        TCHAR buffer[128]{};
        StringCchPrintf(buffer, 128, TEXT("message: %04X\n"), message);
        OutputDebugString(buffer);
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
    SendMessage(this->m_hDlg, TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 20));

    ColourCtrl::staticConstructor();
    ColourCtrl colourCtrl;
    colourCtrl.create(this->m_hDlg);

    return (INT_PTR)TRUE;
}

INT_PTR WinAPISettings::onSetFont(WPARAM wParam, LPARAM lParam) {
    HFONT hFont = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
    SendMessage(this->m_hDlg, WM_SETFONT, (LPARAM)hFont, TRUE);
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
    case IDC_FILEOPEN: {
        this->onFileOpen(wParam, lParam);
    } break;
    case IDC_FILESAVE: {
        this->onFileSave(wParam, lParam);
    } break;
    case IDC_FILESAVEAS: {
        this->onFileSaveAs(wParam, lParam);
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

    // close the dialog
    EndDialog(this->m_hDlg, (LOWORD(wParam)));
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

void WinAPISettings::onFileOpen(WPARAM wParam, LPARAM lParam) {
    OPENFILENAME ofn;   // common dialog box structure
    WCHAR szFile[260];   // buffer for the file name
    HANDLE hf;          // file handle

    // initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = this->m_hDlg;
    ofn.lpstrFile = szFile;
    // set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // display the open dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        SendMessage(GetParent(this->m_hDlg), ID_FILECHANGE, wParam, (LPARAM)((LPCTSTR)szFile));
        SendMessage(GetParent(this->m_hDlg), IDC_FILEOPEN, wParam, lParam);
    }
}

void WinAPISettings::onFileSave(WPARAM wParam, LPARAM lParam) {
    SendMessage(GetParent(this->m_hDlg), IDC_FILESAVE, wParam, lParam);
}

void WinAPISettings::onFileSaveAs(WPARAM wParam, LPARAM lParam) {
    OPENFILENAME ofn;   // common dialog box structure
    WCHAR szFile[260];   // buffer for the file name
    HANDLE hf;          // file handle

    // initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = this->m_hDlg;
    ofn.lpstrFile = szFile;
    // set lpstrFile[0] to '\0' so that GetOpenFileName does not use the contents of szFile to initialize itself
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // display the open dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        SendMessage(GetParent(this->m_hDlg), ID_FILECHANGE, wParam, (LPARAM)((LPCTSTR)szFile));
        SendMessage(GetParent(this->m_hDlg), IDC_FILESAVE, wParam, lParam);
    }
}

BOOL WinAPISettings::create(HWND parent) {
    WinAPISettings::ms_pWnd[WinAPISettings::ms_wndCnt] = this;
    ++WinAPISettings::ms_wndCnt;

    DialogBoxParam(WinAPIShapes::ms_hInstance, MAKEINTRESOURCE(IDD_SETTINGS), parent, WinAPISettings::WndProcClass, (LPARAM)this);

    return TRUE;
}