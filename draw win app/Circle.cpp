#include <Windows.h>
#include <gdiplus.h>
#include "Circle.h"

using namespace Gdiplus;
using namespace std;

// if no other parent constructor is called, the default one will be
Circle::Circle() : Shape() {
	this->m_radius = 0;
};

Circle::Circle(int x, int y, int diameter, int xvel, int yvel) : Shape(x, y, diameter, xvel, yvel) {
	this->m_radius = diameter / 2;
};

void Circle::draw(Graphics& graphics, bool drawID) {
	this->m_radius = this->getWidth() / 2;
	// pen info
	Color penColor = this->getColour();
	double penWidth = 3.0;
	Pen pen(penColor, penWidth);

	// draw circle
	graphics.DrawEllipse(&pen, this->getX() - this->m_radius, this->getY() - this->m_radius, this->m_radius * 2, this->m_radius * 2);

	// draw the selected border if the shape is selected
	if (this->getSelected()) {
		Color c = Color::LightGray;
		float w = 3.0;
		Pen p(c, w);

		RectF border(this->getX() - (this->getWidth() / 2) - 3, this->getY() - (this->getWidth() / 2) - 3, this->getWidth() + 6, this->getWidth() + 6);
		graphics.DrawRectangle(&p, border);
	}

	// call drawID
	if (drawID)
		this->drawID(graphics);
}

wostream& Circle::save(wostream& os) {
	os << "c" << " " << this->getX() << " " << this->getY() << " " << this->m_radius << " " << (int)this->getColour().GetRed() << " " << (int)this->getColour().GetGreen() << " " << (int)this->getColour().GetBlue() << endl;
	return os;
}

wistream& Circle::load(wistream& is) {
	is.ignore(2);

	// set values from text file
	int x, y, radius, red, green, blue;
	is >> x;
	is >> y;
	is >> radius;
	is >> red;
	is >> green;
	is >> blue;

	// set values
	this->setX(x);
	this->setY(y);
	this->m_radius = radius;
	this->setWidth(radius * 2);
	this->setColour(Color(red, green, blue));

	is.ignore(1);

	return is;
}

int Circle::getRadius() {
	return this->m_radius;
}

void Circle::setRadius(int radius) {
	this->m_radius = radius;
}