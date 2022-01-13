#ifndef WINAPISETTINGS_H
#define WINAPISETTINGS_H

#include <Windows.h>

// manages the settings window
class WinAPISettings {
	HWND m_hDlg{};	// a handle to the current window

	// message map
	typedef INT_PTR (WinAPISettings::* MessageHandler)(WPARAM, LPARAM);
	struct MessageMap
	{
		UINT msg;
		MessageHandler pfnHandler;
	};

	static MessageMap ms_msgMap[100];
	static size_t ms_cnt;

	// register handle
	static void hReg(UINT message, MessageHandler hMsg);

	static WinAPISettings* findWindow(HWND);

	static WinAPISettings* ms_pWnd[100];
	static size_t ms_wndCnt;

	// static wndproc
	static INT_PTR CALLBACK WndProcClass(HWND, UINT, WPARAM, LPARAM);

protected:
	// when the dialog is created
	INT_PTR onInitDialog(WPARAM wParam, LPARAM lParam);

	// on command
	INT_PTR onCommand(WPARAM wParam, LPARAM lParam);

	// when the 'ok' button is clicked
	void onOk(WPARAM wParam, LPARAM lParam);

	// then the 'colour' button is clicked
	void onColour(WPARAM wParam, LPARAM lParam);

public:
	// default constructor
	WinAPISettings();

	// saves instance handle and creates dialog window
	BOOL create(HWND parent);

	// static constructor (initializes messages)
	static void staticConstructor();

	// message handler
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif