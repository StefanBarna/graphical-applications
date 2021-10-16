#ifndef SHAPE_H
#define SHAPE_H

#include <Windows.h>
#include <gdiplus.h>
#include <iostream>

class Shape {
	// x and y are the coordinates for the center
	static int m_cnt;	// number of created shapes
	int m_id;			// the ID number of the current instance
	int m_x;			// the instance location on the x axis
	int m_y;			// the instance location on the y axis
	int m_width;		// the width of the instance

public:
	/// <summary>
	/// base constructor
	/// </summary>
	Shape();

	/// <summary>
	/// custom constructor
	/// </summary>
	/// <param name="x">x position</param>
	/// <param name="y">y position</param>
	/// <param name="width">width of shape</param>
	/// <param name="save">false if the shape is temporary, true otherwise</param>
	Shape(int x, int y, int width, bool save = true);

	// virtual means that polymorphism on the draw function is enabled
	// = 0 means the function is PURE virtual, meaning derived classes MUST define this function
	// draws the shape to a window

	/// <summary>
	/// draws the shape to a window
	/// </summary>
	/// <param name="graphics">the graphics being drawn to</param>
	virtual void draw(Gdiplus::Graphics& graphics) = 0; 

	/// <summary>
	/// saves the information of a shape to an ostream
	/// </summary>
	/// <param name="os">the stream to which a shape is saved</param>
	/// <returns>the stream received as parameter</returns>
	virtual std::ostream& save(std::ostream& os) = 0;

	/// <summary>
	/// loads information about a shape from an istream
	/// </summary>
	/// <param name="os">the stream from which information about a shape is taken</param>
	/// <returns>the stream received as parameter</returns>
	virtual std::istream& load(std::istream& is) = 0;

	/// <summary>
	/// returns whether the parameter Shape is overlapping with the current instance
	/// </summary>
	/// <param name="selected">the shape being checked for overlapping</param>
	/// <returns>boolean representing whether the two shapes are overlapping</returns>
	bool overlap(Shape& selected);

	/// <summary>
	/// draws the shape ID in the center of the shape
	/// </summary>
	/// <param name="graphics">the graphics being drawn to</param>
	void drawID(Gdiplus::Graphics& graphics);

	/// <summary>
	/// x getter
	/// </summary>
	/// <returns>the x position of the center of the shape</returns>
	int getX();

	/// <summary>
	/// y getter
	/// </summary>
	/// <returns>the y position of the center of the shape</returns>
	int getY();

	/// <summary>
	/// width getter
	/// </summary>
	/// <returns>the width of the shape</returns>
	int getWidth();

	/// <summary>
	/// ID getter
	/// </summary>
	/// <returns>the shape ID</returns>
	int getID();

	/// <summary>
	/// x setter
	/// </summary>
	/// <param name="x">the new x position</param>
	void setX(int x);

	/// <summary>
	/// y setter
	/// </summary>
	/// <param name="x">the new y position</param>
	void setY(int y);

	/// <summary>
	/// changes the (x,y) position of the shape
	/// </summary>
	/// <param name="x">the x position</param>
	/// <param name="y">the y position</param>
	void setPos(int x, int y);

	/// <summary>
	/// width setter
	/// </summary>
	/// <param name="width">the new width of the shape</param>
	void setWidth(int width);

	/// <summary>
	/// shape count getter
	/// </summary>
	/// <returns>the total number of shapes created</returns>
	static int shapeCnt();
};

#endif