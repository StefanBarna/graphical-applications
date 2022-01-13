#pragma once
#include <Windows.h>
#include <gdiplus.h>
#include "Shape.h"

class Square : public Shape {
public:
	/// <summary>
	/// base constructor
	/// </summary>
	Square();

	/// <summary>
	/// custom constructor
	/// </summary>
	/// <param name="x">x position</param>
	/// <param name="y">y position</param>
	/// <param name="width">width of square</param>
	/// <param name="save">false if the square is temporary, true otherwise</param>
	Square(int x, int y, int width = Shape::defaultWidth, int xvel = 1, int yvel = 1);

	/// <summary>
	/// draws the shape to a window
	/// </summary>
	/// <param name="graphics">the graphics being drawn to</param>
	void draw(Gdiplus::Graphics& graphics, bool drawID = true) override;

	/// <summary>
	/// saves the information of a shape to an ostream
	/// </summary>
	/// <param name="os">the stream to which a shape is saved</param>
	/// <returns>the stream received as parameter</returns>
	std::ostream& save(std::ostream& os) override;

	/// <summary>
	/// loads information about a shape from an istream
	/// </summary>
	/// <param name="os">the stream from which information about a shape is taken</param>
	/// <returns>the stream received as parameter</returns>
	std::istream& load(std::istream& is) override;
};

