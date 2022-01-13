#ifndef WINAPISHAPES_H
#define WINAPISHAPES_H

#include <Windows.h>
#include <fstream>
#include <list>

#include "WinAPISettings.h"
#include "Shape.h"

const int speed = 5;

// manages the main window
class WinAPIShapes {
	// dimensions of the window
	size_t m_height;
	size_t m_width;

	HWND m_hWnd{};		// a handle to the current window (the identifier)
	HWND m_lbhWnd{};	// a handle to a list box on the current window

	// Message Map
	typedef void (WinAPIShapes::* MessageHandler)(WPARAM, LPARAM);
	struct MessageMap
	{
		UINT msg;
		MessageHandler pfnHandler;
	};

	// message map variables
	static MessageMap ms_msgMap[100];
	static size_t ms_cnt;

	// register handle
	static void hReg(UINT message, MessageHandler hMsg);

	static WinAPIShapes* ms_pWnd[100];
	static size_t ms_wndCnt;

	static WinAPIShapes* findWindow(HWND);

	// private variables
	std::list<Shape*> m_shapes{};		// list of shapes
	std::string m_filename{};			// save file name
	std::fstream m_file{};				// save file fstream
	
	struct selectedShape
	{
		Shape* s;	// shape selected
		int x;		// original location of shape x
		int y;		// original location of shape y
	};
	std::list<selectedShape> m_selected{};

	static LRESULT CALLBACK WndProcClass(HWND, UINT, WPARAM, LPARAM);
	static constexpr WCHAR ms_szWindowClass[100]{L"DRAWWINAPP"};		// the main window class name

	// something cool
	TCHAR m_windowTitle[100]{TEXT("Default Window Title")};

	// left button is being clicked
	bool m_mousedown;
	int m_movedX;
	int m_movedY;

	// timer button
	HWND m_timerBtn;
	bool m_timerOn;

protected:
	// registers the window class
	ATOM registerClass() const;

	// paint to screen
	void onPaint(WPARAM wParam, LPARAM lParam);

	// startup
	void onCreate(WPARAM wParam, LPARAM lParam);

	// window destruction
	void onDestroy(WPARAM wParam, LPARAM lParam);

	// when the left mouse button is pushed
	void onLButtonDown(WPARAM wParam, LPARAM lParam);

	// when the left mouse button is released
	void onLButtonUp(WPARAM wParam, LPARAM lParam);

	// when the right mouse button is pushed
	void onRButtonDown(WPARAM wParam, LPARAM lParam);

	// when the mouse is moved
	void onMouseMove(WPARAM wParam, LPARAM lParam);

	// when the mouse wheel is scrolled
	void onMouseWheel(WPARAM wParam, LPARAM lParam);

	// when a menu window is called
	void onCommand(WPARAM wParam, LPARAM lParam);

	// when a char key is clicked
	void onChar(WPARAM wParam, LPARAM lParam);

	// when backspace is clicked
	void onDelete();

	// when escape key is clicked
	void onEscape();

	// on every timer tick
	void onTimer(WPARAM wParam, LPARAM lParam);

	// change client area
	void onNCCalcSize(WPARAM wParam, LPARAM lParam);

	// load file
	void loadFile();

	// save file
	void saveFile();

	// TODO: make the template only accept classes derived from Shape
	// create a shape
	template<typename T>
	void createShape(int x, int y) {
		// create a shape of type T
		int width = Shape::defaultWidth;
		T* c = new T(x, y, width);

		// save the shape to the list of shapes
		this->m_shapes.push_back(c);

		// redraw the area in which you made the shape
		RECT shapeArea{ x - (c->getWidth() / 2) - 3, y - (c->getWidth() / 2) - 3, x + (c->getWidth() / 2) + 3, y + (c->getWidth() / 2) + 3 }; // gets the attributes one by one
		InvalidateRect(this->m_hWnd, &shapeArea, true); // only modifies a certain rectangle of the screen

		// add a new element in the list (a new string)
		TCHAR buffer[128]{};
		StringCchPrintf(buffer, 128, TEXT("%d. x = [%d], y = [%d]  --  Added a new shape with ID [%d]!"), this->m_shapes.size(), c->getX(), c->getY(), c->getID());
		SendMessage(this->m_lbhWnd, LB_INSERTSTRING, 0, (LPARAM)buffer);
	}

	// move shape s by x and y amount
	void moveShape(LPARAM lParam, Shape* s, int x, int y);

	// deselect shape
	void releaseShape(LPARAM lParam);

	// resize shape
	void resizeShape(LPARAM lParam, WPARAM wParam);

	// check if the shape and the mouse overlap
	bool isOnShape(int x, int y);

	// creates a rectangle that encompasses the two parameter rectangles
	RECT* fusedRect(RECT r1, RECT r2);

	// check for collisions
	void collide(Shape* s);

	// check for selected shapes
	Shape* select();

public:
	void setWindowTitle(const TCHAR* title);

	static bool ms_isRegistered;

	static HINSTANCE ms_hInstance;

	// saves instance handle and creates main window
	BOOL create(int nCmdShow);

	// sets the window handle
	void setWindow(HWND hWnd) { if (this->m_hWnd == 0) this->m_hWnd = hWnd; } // a Bad Workaround (temporary)

	// default constructor
	WinAPIShapes();

	// static constructor
	static void staticConstructor();

	// message handler
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif