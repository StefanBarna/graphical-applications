#ifndef WINAPISHAPES_H
#define WINAPISHAPES_H

#include <Windows.h>
#include <fstream>

#include "Shape.h"

// Manages a windows
class WinAPIShapes {
	HWND m_hWnd;	// a handle to the current window (the identifier)
	HWND m_lbhWnd;	// a handle to a list box on the current window

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

	// private variables
	std::list<Shape*> m_shapes;		// list of shapes
	std::string m_filename;			// save file name
	std::fstream m_file;			// save file fstream
	Shape* m_selected;
	int m_selectedx, m_selectedy;	// location of selected object before being moved
	// selected shape information

	static LRESULT CALLBACK WndProcClass(HWND, UINT, WPARAM, LPARAM);
	static constexpr WCHAR ms_szWindowClass[100]{L"DRAWWINAPP"};		// the main window class name

protected:
	// registers the window class
	ATOM registerClass() const;

	// paint to screen
	void onPaint(WPARAM wParam, LPARAM lParam);

	// startup
	void onCreate(WPARAM wParam, LPARAM lParam);

	// window destruction
	void onDestroy(WPARAM wParam, LPARAM lParam);

	// when the left mouse button is clicked
	void onLButtonDown(WPARAM wParam, LPARAM lParam);

	// when the left mouse button is released
	void onLButtonUp(WPARAM wParam, LPARAM lParam);

	// when the mouse is moved
	void onMouseMove(WPARAM wParam, LPARAM lParam);

	// when the mouse wheel is scrolled
	void onMouseWheel(WPARAM wParam, LPARAM lParam);

	// load file
	void loadFile();

	// save file
	void saveFile();

	// TODO: make the template only accept classes derived from Shape | Contracts C++20
	// use a class factory instead of a template
	// create a shape
	template<typename T>
	void createShape(int x, int y) {
		// create a shape of type T
		int width = Shape::defaultWidth;
		T* c = new T(x, y);

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

	// select shape
	void selectShape();

	// move shape
	void moveShape(LPARAM lParam);

	// deselect shape
	void releaseShape(LPARAM lParam);

	// resize shape
	void resizeShape(LPARAM lParam, WPARAM wParam);

	// check if the shape and the mouse overlap
	bool isOnShape(int x, int y);

	// creates a rectangle that encompasses the two parameter rectangles
	RECT* fusedRect(RECT r1, RECT r2);

public:
	static bool ms_isRegistered;

	static HINSTANCE ms_hInstance;

	// saves instance handle and creates main window
	BOOL create(int nCmdShow);

	// sets the window handle
	void setWindow(HWND hWnd) { if (this->m_hWnd == 0) this->m_hWnd = hWnd; } // a Bad Workaround (temporary)

	// default constructor
	WinAPIShapes();

	// default constructor
	static void staticConstructor();

	// message handler
	LRESULT CALLBACK WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif