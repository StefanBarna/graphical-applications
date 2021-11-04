#include <Windows.h>
#include <gdiplus.h>
#include "Square.h"

using namespace Gdiplus;
using namespace std;

Square::Square() : Shape() {};

Square::Square(int x, int y, int width, bool save) : Shape(x, y, width, save) {};

void Square::draw(Gdiplus::Graphics& graphics) {
	// pen info
	Color penColor = this->getColour();
	double penWidth = 3.0;
	Pen pen(penColor, penWidth);

	// draw square
	RectF sqr(this->getX() - (this->getWidth() / 2), this->getY() - (this->getWidth() / 2), this->getWidth(), this->getWidth());
	graphics.DrawRectangle(&pen, sqr);

	// call drawID
	this->drawID(graphics);
}

ostream& Square::save(ostream& os) {
	os << "s" << " " << this->getX() << " " << this->getY() << " " << this->getWidth() << " " << (int)this->getColour().GetRed() << " " << (int)this->getColour().GetGreen() << " " << (int)this->getColour().GetBlue() << endl;
	return os;
}

istream& Square::load(istream& is) {
	is.ignore(2);

	// set values from text file
	int x, y, width, red, green, blue;
	is >> x;
	is >> y;
	is >> width;
	is >> red;
	is >> green;
	is >> blue;

	// set values
	this->setX(x);
	this->setY(y);
	this->setWidth(width);
	this->setColour(Color(red, green, blue));

	is.ignore(1);

	return is;
}