#ifndef ColourCtrl_H
#define ColourCtrl_H

#include <Windows.h>
#include <string>
#include "WinAPISettings.h"

//void RegisterControl(HINSTANCE hInstance);
//
//HWND CreateCtrl(HWND hWnd);
//
//LRESULT CALLBACK ColourCtrl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class ColourCtrl {
	HWND m_hWnd{};	// a handle to the current window (identifier)

	// message map
	typedef INT_PTR(ColourCtrl::* MessageHandler)(WPARAM, LPARAM);
	struct MessageMap
	{
		UINT msg;
		MessageHandler pfnHandler;
	};

	static MessageMap ms_msgMap[100];
	static size_t ms_cnt;

	// register handle
	static void hReg(UINT, MessageHandler);

	static ColourCtrl* findWindow(HWND);

	// array of registered custom Ctrl windows
	static ColourCtrl* ms_pWnd[100];
	static size_t ms_wndCnt;

	// static wndproc
	static LRESULT CALLBACK WndProcClass(HWND, UINT, WPARAM, LPARAM);

	// class variables
	std::wstring colourCode;

protected:
	// messages on creation of custom Ctrl
	INT_PTR onCreate(WPARAM wParam, LPARAM lParam);

	// WM_GETDLGCODE
	INT_PTR onGetDlgCode(WPARAM wParam, LPARAM lParam);

	// input character handling
	INT_PTR onChar(WPARAM wParam, LPARAM lParam);

	// draw
	INT_PTR onPaint(WPARAM wParam, LPARAM lParam);

	// when you click on the control
	INT_PTR onLButtonDown(WPARAM wParam, LPARAM lparam);

public:
	// constructor
	ColourCtrl();

	// saves instance handle and creates dialog window
	BOOL create(HWND);

	// static constructor - initializes messages
	static void staticConstructor();

	// message handler
	LRESULT CALLBACK WndProc(UINT, WPARAM, LPARAM);
};

#endif