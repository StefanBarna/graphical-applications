#include <strsafe.h>
#include "Shape.h"
using namespace Gdiplus;

int Shape::ms_cnt = 0;
int Shape::defaultWidth = 50;
Color Shape::defaultBorderColour = Color(0, 0, 0);

Shape::Shape() {
	Shape::ms_cnt++;
	this->m_id = Shape::ms_cnt;
	this->m_x = 0;
	this->m_y = 0;
	this->m_width = this->defaultWidth;
	this->m_colour = this->defaultBorderColour;
	this->m_xvel = 1;
	this->m_yvel = 1;
	this->m_isSelected = false;
}

Shape::Shape(int x, int y, int width, int xvel, int yvel) {
	Shape::ms_cnt++;
	this->m_id = Shape::ms_cnt;
	this->m_x = x;
	this->m_y = y;
	this->m_width = width;
	this->m_colour = this->defaultBorderColour;
	this->m_xvel = xvel;
	this->m_yvel = yvel;
	this->m_isSelected = false;
}

Shape::~Shape() {
	Shape::ms_cnt--;
}

bool Shape::overlap(Shape& selected) {
	bool overlap = false;

	if (((this->getX() - (this->getWidth() / 2)) >= (selected.getX() + (selected.getWidth() / 2))
		|| (this->getX() + (this->getWidth() / 2)) <= (selected.getX() - (selected.getWidth() / 2)))
		|| ((this->getY() - (this->getWidth() / 2)) >= (selected.getY() + (selected.getWidth() / 2))
		|| (this->getY() + (this->getWidth() / 2)) <= (selected.getY() - (selected.getWidth() / 2))))
		overlap = false;
	else if (this != &selected)
		overlap = true;

	return overlap;
}

void Shape::drawID(Graphics& graphics) {
	// parameters for text
	Font myFont(L"Tahoma", 14);
	SolidBrush brush(Color::Black);
	TCHAR ID[31]; // maximum ID is 10^31
	StringCchPrintf(ID, 31, TEXT("%d"), this->m_id);

	// measure the size of the string
	RectF layoutRect(0.0f, 0.0f, 100.0f, 50.0f);
	StringFormat format;
	RectF boundRect;
	graphics.MeasureString(ID, -1, &myFont, layoutRect, &format, &boundRect);
	
	// draw the string to the circle
	PointF origin(this->m_x - (boundRect.Width / 2), this->m_y - (boundRect.Height / 2));
	graphics.DrawString(ID, -1, &myFont, origin, &brush);
}

int Shape::getX() {
	return this->m_x;
}

int Shape::getY() {
	return this->m_y;
}

int Shape::getWidth() {
	return this->m_width;
}

int Shape::getID() {
	return this->m_id;
}

Color Shape::getColour() {
	return this->m_colour;
}

bool Shape::getSelected() {
	return this->m_isSelected;
}

int Shape::getXVel() {
	return this->m_xvel;
}

int Shape::getYVel() {
	return this->m_yvel;
}

void Shape::setX(int x) {
	this->m_x = x;
}

void Shape::setY(int y) {
	this->m_y = y;
}

void Shape::setWidth(int width) {
	this->m_width = width;
}

void Shape::setPos(int x, int y) {
	this->m_x = x;
	this->m_y = y;
}

void Shape::setColour(Color colour) {
	this->m_colour = colour;
}

int Shape::shapeCnt() {
	return Shape::ms_cnt;
}

void Shape::move(int speed) {
	this->m_x += this->m_xvel * speed;
	this->m_y += this->m_yvel * speed;
}

void Shape::setVel(int x, int y) {
	this->m_xvel = x * this->m_xvel;
	this->m_yvel = y * this->m_yvel;
}

void Shape::setSelected(bool isSelected) {
	this->m_isSelected = isSelected;
}

void Shape::resetCnt() {
	Shape::ms_cnt = 0;
}