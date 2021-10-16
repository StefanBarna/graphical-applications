#include "Utilities.h"

Shape* shapeFactory(char flag) {
	Shape* s = nullptr;

	switch (flag) {
	case 'c':
		s = new Circle();
		break;
	case 's':
		s = new Square();
		break;
	default:
		throw "error: unknown tag";
		break;
	}

	return s;
}