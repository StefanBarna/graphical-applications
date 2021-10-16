#include <Windows.h>
#include <gdiplus.h>
#include "Circle.h"

using namespace Gdiplus;
using namespace std;

// if no other parent constructor is called, the default one will be
Circle::Circle() : Shape() {
	this->m_radius = 0;
};

Circle::Circle(int x, int y, int radius, bool save) : Shape(x, y, radius*2, save) {
	this->m_radius = radius;
};

void Circle::draw(Graphics& graphics) {
	this->m_radius = this->getWidth() / 2; // TODO: this was a quick fix, fix the code elsewhere

	// pen info
	Color penColor = Color::Blue;
	double penWidth = 3.0;
	Pen pen(penColor, penWidth);

	// draw circle
	graphics.DrawEllipse(&pen, this->getX() - this->m_radius, this->getY() - this->m_radius, this->m_radius * 2, this->m_radius * 2);

	// call drawID
	this->drawID(graphics);
}

ostream& Circle::save(ostream& os) {
	os << "c" << " " << this->getX() << " " << this->getY() << " " << this->m_radius << endl;
	return os;
}

istream& Circle::load(istream& is) {
	is.ignore(2);

	// set values from text file
	int x, y, radius;
	is >> x;
	is >> y;
	is >> radius;

	// set values
	this->setX(x);
	this->setY(y);
	this->m_radius = radius;
	this->setWidth(radius * 2);

	is.ignore(1);

	return is;
}

int Circle::getRadius() {
	return this->m_radius;
}

void Circle::setRadius(int radius) {
	this->m_radius = radius;
}