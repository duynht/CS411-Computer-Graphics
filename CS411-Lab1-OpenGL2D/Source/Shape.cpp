#include "Shape.h"

BresenhamLine::BresenhamLine(GLint x0, GLint y0, GLint x1, GLint y1) {
	type = CGType::BRES_LINE;
	
	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1;
	this->y1 = y1;
}

DDALine::DDALine(GLint x0, GLint y0, GLint x1, GLint y1) {
	type = CGType::DDA_LINE;
	
	this->x0 = x0;
	this->y0 = y0;
	this->x1 = x1;
	this->y1 = y1;
}

MidpointCircle::MidpointCircle(GLint x, GLint y, GLint r) {
	type = CGType::CIRCLE;
	
	xCenter = x;
	yCenter = y;
	radius = r;
}

MidpointEllipse::MidpointEllipse(GLint x, GLint y, GLint a, GLint b) {
	type = CGType::ELLIPSE;
	
	xCenter = x;
	yCenter = y;
	rx = a;
	ry = b;
}

MidpointParabola::MidpointParabola(GLint x, GLint y, GLint focalLength, GLint xBound) {
	type = CGType::PARABOLA;
	
	xFocus = x;
	yFocus = y;
	p = focalLength;
	bound = xBound;
}

MidpointHyperbola::MidpointHyperbola(GLint x, GLint y, GLint a, GLint b, GLint yBound) {
	type = CGType::HYPERBOLA;
	
	xCenter = x;
	yCenter = y;
	rx = a;
	ry = b;
	bound = yBound;
}

void Shape::draw() {
	GLuint vertArray;
	glCreateVertexArrays(1, &vertArray);
	glBindVertexArray(vertArray);
	GLuint buffer;
	glCreateBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glNamedBufferStorage(buffer, sizeof(GLint) * positions.size(), positions.data(), 0);

	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(GLint) * 2, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(glPrimitive, 0, numPoints);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DDALine::draw() {
	positions.clear();
	GLint dy = y1 - y0;
	GLint dx = x1 - x0;
	numPoints = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	//slope m
	GLdouble incX = dx / (double) numPoints;
	GLdouble incY = dy / (double) numPoints;
	
	GLdouble y = y0;
	GLdouble x = x0;

	for (GLint i = 0; i <= numPoints; i++) {
		positions.push_back(x);
		positions.push_back(y);
		x += incX;
		y += incY;
	}

	Shape::draw();
}

void DDALine::glDraw() {
	Shape::positions.clear();
	Shape::positions.push_back(x0);
	Shape::positions.push_back(y0);
	Shape::positions.push_back(x1);
	Shape::positions.push_back(y1);
	Shape::numPoints = 2;
	Shape::glPrimitive = GL_LINES;
	Shape::draw();
	Shape::glPrimitive = GL_POINTS;
}

void BresenhamLine::draw() {
	Shape::positions.clear();
	GLint decision, incE, incNE;

	GLint _x0 = x0;
	GLint _y0 = y0;
	GLint _x1 = x1;
	GLint _y1 = y1;

	GLint dx = _x1 - _x0;
	GLint dy = _y1 - _y0;

	GLint sx = dx < 0 ? -1 : (dx > 0 ? 1 : 0);
	GLint sy = dy < 0 ? -1 : (dy > 0 ? 1 : 0);

	dx = abs(dx);
	dy = abs(dy);

	_x0 += sx;
	_y0 += sy;

	GLint y = _y0;
	GLint x = _x0;

	if (dy > dx) {
		incE = 2 * dx;
		incNE = 2*dx - 2*dy;
		decision = 2*dx - dy;

		do {
			Shape::numPoints++;
			Shape::positions.push_back(x);
			Shape::positions.push_back(y);
			if (decision <= 0)
				decision += incE;
			else {
				decision += incNE;
				x += sx;
			}
			y += sy;
		} while (y != _y1);
	}
	else {
		incE = 2 * dy;
		incNE = 2*dy - 2*dx;
		decision = 2*dy - dx;

		do {
			Shape::numPoints++;
			Shape::positions.push_back(x);
			Shape::positions.push_back(y);
			if (decision <= 0)
				decision += incE;
			else {
				decision += incNE;
				y += sy;
			}
			x += sx;
		} while (x != _x1);
	}
	
	Shape::draw();
}

void BresenhamLine::glDraw() {
	Shape::positions.clear();
	Shape::positions.push_back(x0);
	Shape::positions.push_back(y0);
	Shape::positions.push_back(x1);
	Shape::positions.push_back(y1);
	Shape::numPoints = 2;
	Shape::glPrimitive = GL_LINES;
	Shape::draw();
	Shape::glPrimitive = GL_POINTS;
}

void MidpointCircle::draw() {
	Shape::positions.clear();
	GLint x = radius;
	GLint y = 0;
	GLint p = 0;

	if (radius == 0) {
		Shape::positions.push_back(xCenter);
		Shape::positions.push_back(yCenter);

		Shape::numPoints += 1;
	}

	while (x >= y) {

		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter + y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter + y);

		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter - y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter - y);

		Shape::numPoints += 4;
		
		if (x != y) {
			Shape::positions.push_back(xCenter + y);
			Shape::positions.push_back(yCenter + x);

			Shape::positions.push_back(xCenter - y);
			Shape::positions.push_back(yCenter + x);

			Shape::positions.push_back(xCenter + y);
			Shape::positions.push_back(yCenter - x);

			Shape::positions.push_back(xCenter - y);
			Shape::positions.push_back(yCenter - x);

			Shape::numPoints += 4;
		}
		if (p <= 0) {
			y++;
			p += 2 * y + 1;
		}
		else {
			x--;
			p -= 2 * x + 1;
		}
	}

	Shape::draw();

}

void MidpointEllipse::draw() {
	Shape::positions.clear();
	GLdouble x = 0;
	GLdouble y = ry;

	GLdouble d1 = ((double)ry * (double)ry) - ((double)rx * (double)rx * (double)ry) + (0.25 * (double)rx * (double)rx);
	GLdouble dx = 2 * (double)ry * (double)ry * x;
	GLdouble dy = 2 * (double)rx * (double)rx * y;

	while (dx < dy) {
		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter + y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter + y);


		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter - y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter - y);

		Shape::numPoints += 4;

		if (d1 < 0) {
			x++;
			dx += 2 * (double)ry * (double)ry;
			d1 += dx + (double)ry * (double)ry;
		}
		else {
			x++;
			y--;
			dx += 2 * (double)ry * (double)ry;
			dy -= 2 * (double)rx * (double)rx;
			d1 += dx - dy + (double)ry * (double)ry;
		}
	}

	GLdouble d2 = (((double)ry * (double)ry) * ((x + 0.5) * (x + 0.5))) +
		(((double)rx * (double)rx) * ((y - 1) * (y - 1))) -
		((double)rx * (double)rx * (double)ry * (double)ry);

	while (y >= 0) {
		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter + y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter + y);


		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter - y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter - y);

		Shape::numPoints += 4;

		if (d2 > 0) {
			y--;
			dy -= 2 * (double)rx * (double)rx;
			d2 += (double)rx * (double)rx - dy;
		}
		else {
			y--;
			x++;
			dx += 2 * (double)ry * (double)ry;
			dy -= 2 * (double)rx * (double)rx;
			d2 += dx - dy + (double)rx * (double)rx;
		}
	}

	Shape::draw();
}

void MidpointParabola::draw() {
	Shape::positions.clear();
	GLint p2 = 2 * p;
	GLint p4 = 2 * p2;
	GLint x = 0;
	GLint y = 0;
	GLint d = 1 - p;
	while ((y < p) && (x <= bound)) {
		Shape::positions.push_back(xFocus + x);
		Shape::positions.push_back(yFocus + y);

		Shape::positions.push_back(xFocus + x);
		Shape::positions.push_back(yFocus - y);

		Shape::numPoints +=2 ;
		if (d >= 0) {
			x += 1;
			d -= p2;
		}
		y++;
		d += 2 * y + 1;
	}
	if (d == 1) {
		d = 1 - p4;
	}
	else {
		d = 1 - p2;
	}

	while (x <= bound) {
		Shape::positions.push_back(xFocus + x);
		Shape::positions.push_back(yFocus + y);

		Shape::positions.push_back(xFocus + x);
		Shape::positions.push_back(yFocus - y);

		Shape::numPoints += 2;
		if (d <= 0) {
			y++;
			d += 4 * y;
		}

		x++;
		d -= p4;
	}

	Shape::draw();
}

void MidpointHyperbola::draw() {
	Shape::positions.clear();
	GLint x = rx;
	GLint y = 0;
	GLint rxSq = rx * rx;
	GLint rySq = ry * ry;
	GLint rxSq2 = rxSq + rxSq;
	GLint rySq2 = rySq + rySq;
	GLint rxSq4 = rxSq2 + rxSq2;
	GLint rySq4 = rySq2 + rySq2;
	GLint xSlope = rySq4 * (x + 1);
	GLint ySlope = rxSq4;
	GLint midA = rxSq / 2;
	GLint midB = rySq / 2;
	GLint d = rxSq2 - rySq * (1 + 2 * rx) + midB;
	while ((d < xSlope) && (y <= bound)) {
		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter + y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter + y);


		Shape::positions.push_back(xCenter + x);
		Shape::positions.push_back(yCenter - y);

		Shape::positions.push_back(xCenter - x);
		Shape::positions.push_back(yCenter - y);

		Shape::numPoints += 4;
		if (d >= 0) {
			d -= xSlope;
			x++;
			xSlope += rySq4;
		}
		d += rxSq2 + ySlope;
		y++;
		ySlope += rxSq4;
	}

	d -= (xSlope + ySlope) / 2 + (rxSq + rySq) - midB - midA;

	if (rx > ry) {
		while (y <= bound) {
			Shape::positions.push_back(xCenter + x);
			Shape::positions.push_back(yCenter + y);

			Shape::positions.push_back(xCenter - x);
			Shape::positions.push_back(yCenter + y);


			Shape::positions.push_back(xCenter + x);
			Shape::positions.push_back(yCenter - y);

			Shape::positions.push_back(xCenter - x);
			Shape::positions.push_back(yCenter - y);

			Shape::numPoints += 4;

			if (d <= 0) {
				d += ySlope;
				y++;
				ySlope += rxSq4;
			}

			d -= rySq2 + xSlope;
			x++;
			xSlope += rySq4;
		}
	}

	Shape::draw();
}
