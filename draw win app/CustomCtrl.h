#ifndef CUSTOMCTRL_H
#define CUSTOMCTRL_H

#include <Windows.h>
#include "WinAPISettings.h"

/*class CustomCtrl {
	HWND m_hWnd{};	// a handle to the current window (identifier)

	// message map
	typedef INT_PTR(CustomCtrl::* MessageHandler)(WPARAM, LPARAM);
	struct MessageMap
	{
		UINT msg;
		MessageHandler pfnHandler;
	};

	static MessageMap ms_msgMap[100];
	static size_t ms_cnt;

	// register handle
	static void hReg(UINT, MessageHandler);

	static CustomCtrl* findWindow(HWND);

	// array of registered custom control windows
	static CustomCtrl* ms_pWnd[100];
	static size_t ms_wndCnt;

	// static wndproc
	static INT_PTR CALLBACK WndProcClass(HWND, UINT, WPARAM, LPARAM);

protected:
	// messages on creation of custom control
	INT_PTR CALLBACK onSetFont(WPARAM wParam, LPARAM lParam);

	// draw
	INT_PTR CALLBACK onPaint(WPARAM wParam, LPARAM lParam);

public:
	// constructor
	CustomCtrl();

	// saves instance handle and creates dialog window
	BOOL create(HWND);

	// static constructor - initializes messages
	static void staticConstructor();

	// message handler
	LRESULT CALLBACK WndProc(UINT, WPARAM, LPARAM);
};*/

#endif