#include <Windows.h>
#include <gdiplus.h>
#include "Square.h"

using namespace Gdiplus;
using namespace std;

Square::Square() : Shape() {};

Square::Square(int x, int y, int width, int xvel, int yvel) : Shape(x, y, width, xvel, yvel) {};

void Square::draw(Gdiplus::Graphics& graphics, bool drawID) {
	// pen info
	Color penColor = this->getColour();
	float penWidth = 3.0;
	Pen pen(penColor, penWidth);

	// draw square
	RectF sqr(this->getX() - (this->getWidth() / 2), this->getY() - (this->getWidth() / 2), this->getWidth(), this->getWidth());
	graphics.DrawRectangle(&pen, sqr);

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