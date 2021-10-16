#ifndef CIRCLE_H
#define CIRCLE_H

#include <iostream>
#include "Shape.h"

class Circle : public Shape {

	// radius of circle
	int m_radius;

public:
	/// <summary>
	/// base constructor
	/// </summary>
	Circle();

	/// <summary>
	/// custom constructor
	/// </summary>
	/// <param name="x">x position</param>
	/// <param name="y">y position</param>
	/// <param name="radius">radius of circle</param>
	/// <param name="save">false if the square is temporary, true otherwise</param>
	Circle(int x, int y, int radius = 25, bool save = true);

	// override expresses an interest in using a virtual function from a parent class
	// it will show the function as an error if there isn't a function with the same prototype
	
	/// <summary>
	/// draws the shape to a window
	/// </summary>
	/// <param name="graphics">the graphics being drawn to</param>
	void draw(Gdiplus::Graphics& graphics) override;

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

	/// <summary>
	/// radius getter
	/// </summary>
	/// <returns>the radius of the current instance</returns>
	int getRadius();

	/// <summary>
	/// radius setter
	/// </summary>
	/// <param name="radius">the new radius</param>
	void setRadius(int radius);
};

#endif