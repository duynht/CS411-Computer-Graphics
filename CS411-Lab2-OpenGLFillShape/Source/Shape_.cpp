#include "Shape.h"

#define euclideanDistance(xA, yA, xB, yB) ceil(sqrt((xA - xB) ^ 2 + (yA - yB) ^ 2))

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

MidpointCircle::MidpointCircle(GLint xO, GLint yO, GLint xA, GLint yA) {
	type = CGType::CIRCLE;
	xCenter = xO;
	yCenter = yO;
	radius = hypot(abs(xO - xA), abs(yO - yA));
}

MidpointEllipse::MidpointEllipse(GLint x, GLint y, GLint a, GLint b) {
	type = CGType::ELLIPSE;

	xCenter = x;
	yCenter = y;
	rx = a;
	ry = b;
}

MidpointEllipse::MidpointEllipse(GLint xF0, GLint yF0, GLint xF1, GLint yF1, GLint xE, GLint yE) {
	xCenter = (xF0 + xF1) / 2;
	yCenter = (yF0 + yF1) / 2;

	GLint EF0 = hypot(abs(xE - xF0), abs(yE - yF0));
	GLint EF1 = hypot(abs(xE - xF1), abs(yE - yF1));
	rx = (EF0 + EF1) / 2;

	GLuint c = hypot(abs(xF0 - xF1), abs(yF0 - yF1)) / 2;
	
	//angle = atan2(1LL * xF0 * yCenter - 1LL * yF0 * xCenter, 1LL * xF0 * xCenter + 1LL * yF0 * yCenter);
	angle = atan2((yF1 - yCenter) / (double) (xF1 - xCenter), 1.0);
		
	if (rx < c) {
		std::swap(rx, c);
	}

	ry = sqrt((1LL * rx - 1LL * c) * (1LL * rx + 1LL * c));

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

CGRectangle::CGRectangle(GLint xUL, GLint yUL, GLint xBR, GLint yBR) {
	type = CGType::RECTANGLE;

	x[0] = xUL;
	y[0] = yUL;
	x[2] = xBR;
	y[2] = yBR;

	x[1] = x[2];
	y[1] = y[0];

	x[3] = x[0];
	y[3] = y[2];

	//When drawing filled objects, however, "slice notation" is used.
	x[3]++;
	y[3]++;
}

CGPolygon::CGPolygon(std::vector<int> x, std::vector<int> y) {
	type = CGType::POLYGON;

	this->x = x;
	this->y = y;

	//When drawing filled objects, however, "slice notation" is used.
	*((this->x).end() - 1)++;
	*((this->y).end() - 1)++;
}

//----------------------

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
	GLdouble incX = dx / (double)numPoints;
	GLdouble incY = dy / (double)numPoints;

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
		incNE = 2 * dx - 2 * dy;
		decision = 2 * dx - dy;

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
		incNE = 2 * dy - 2 * dx;
		decision = 2 * dy - dx;

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

	GLdouble d1 = ( (double) ry * (double) ry ) - ( (double) rx * (double) rx * (double) ry) + (0.25 * (double) rx * (double) rx);
	GLdouble dx = 2 * (double) ry * (double) ry * x;
	GLdouble dy = 2 * (double) rx * (double) rx * y;

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
			dx += 2 * (double) ry * (double) ry;
			d1 += dx + (double) ry * (double) ry;
		}
		else {
			x++;
			y--;
			dx += 2 * (double) ry * (double) ry;
			dy -= 2 * (double) rx * (double) rx;
			d1 += dx - dy + (double) ry * (double) ry;
		}
	}

	GLdouble d2 = (( (double) ry * (double) ry) * ((x + 0.5) * (x + 0.5))) +
		(( (double) rx * (double) rx) * ((y - 1) * (y - 1))) -
		( (double) rx * (double) rx * (double) ry * (double) ry);

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
			dy -= 2 * (double) rx * (double) rx;
			d2 += (double) rx * (double) rx - dy;
		}
		else {
			y--;
			x++;
			dx += 2 * (double) ry * (double) ry;
			dy -= 2 * (double) rx * (double) rx;
			d2 += dx - dy + (double) rx * (double) rx;
		}
	}

	auto forwardTranslate = glm::translate(glm::dmat4(1), glm::dvec3(xCenter, yCenter, 0.0));
	auto rotation = glm::rotate(forwardTranslate, angle, glm::dvec3(0.0, 0.0, 1.0));
	auto transform = glm::translate(rotation, glm::dvec3(-xCenter, -yCenter, 0.0));

	for (auto i = 0; i < Shape::positions.size() - 1; i += 2) {
		auto vec = glm::vec3(transform * glm::vec4((double)Shape::positions[i], (double)Shape::positions[i + 1], 0.0, 1.0));
		Shape::positions[i] = vec[0];
		Shape::positions[i + 1] = vec[1];
	}
	std::cout << angle * 180 / M_PI<< std::endl;
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

		Shape::numPoints += 2;
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

void CGRectangle::draw() {
	Shape::numPoints = 4;
	Shape::positions.clear();
	for (size_t i = 0; i < Shape::numPoints; i++) {
		Shape::positions.push_back(x[i]);
		Shape::positions.push_back(y[i]);
	}
	Shape::glPrimitive = GL_LINE_LOOP;

	Shape::draw();
}

void CGPolygon::draw() {
	Shape::numPoints = x.size();
	Shape::positions.clear();
	for (size_t i = 0; i < Shape::numPoints; i++) {
		Shape::positions.push_back(x[i]);
		Shape::positions.push_back(y[i]);
	}
	Shape::glPrimitive = GL_LINE_LOOP;
	Shape::draw();
}

//-------------------------

void Shape::fill(const GLint &seedX, const GLint &seedY, const GLint &width, const GLint &height) {
	//std::queue<std::pair<GLint, GLint>> q;
	//std::vector<bool> visited(width * height + 1);
	//visited[seedX*height + seedY] = true;
	//q.push({ seedX, seedY });
	//GLint* pixel = new GLint();
	//while (!q.empty()) {
	//	auto pos = q.front();
	//	q.pop();
	//	glReadPixels(pos.first, pos.second, 1, 1, GL_ALPHA, GL_INT, pixel);
	//	if (!*pixel) {
	//		fillPositions.push_back(pos.first);
	//		fillPositions.push_back(pos.second);

	//		if (pos.first - 1 > 0 && !visited[(pos.first - 1)*height + pos.second]) {
	//			q.push({ pos.first - 1, pos.second });
	//			visited[(pos.first - 1) * height + pos.second] = true;
	//		}
	//		if (pos.first + 1 < width && !visited[(pos.first + 1)*height + pos.second]) {
	//			q.push({ pos.first + 1, pos.second });
	//			visited[(pos.first + 1) * height + pos.second] = true;
	//		}
	//		if (pos.second - 1 > 0 && !visited[pos.first*height + (pos.second - 1)]) {
	//			q.push({ pos.first, pos.second - 1 });
	//			visited[pos.first * height + (pos.second - 1)] = true;
	//		}
	//		if (pos.second + 1 < height && !visited[pos.first*height + (pos.second + 1)]) {
	//			q.push({ pos.first, pos.second + 1 });
	//			visited[pos.first * height + (pos.second + 1)] = true;
	//		}
	//	}
	//}

	//delete pixel;
	int x1;
	bool spanAbove, spanBelow;
	GLfloat pixel[4];
	std::vector<bool> visited(width * height);

	std::stack<std::pair<GLint,GLint>> stack;
	stack.push({ seedX, seedY });
	
	while (!stack.empty()) {
		auto pos = stack.top();
		stack.pop();
		glReadPixels(pos.first, height - pos.second, 1, 1, GL_RGBA, GL_FLOAT, pixel);
		x1 = pos.first;
		
		while(!(visited[x1*height + pos.second] || pixel[3]) && --x1 >= 0) {
			glReadPixels(x1, height - pos.second, 1, 1, GL_RGBA, GL_FLOAT, pixel);
		}

		spanAbove = spanBelow = false;

		while(true) {
			if (++x1 < width) {
				glReadPixels(x1, height - pos.second, 1, 1, GL_RGBA, GL_FLOAT, pixel);
				if (visited[x1 * height + pos.second] || pixel[3]) {
					break;
				}
			}
			else {
				break;
			}
			fillPositions.push_back(x1);
			fillPositions.push_back(pos.second);
			visited[x1 * height + pos.second] = true;

			if (pos.second > 0) {
				glReadPixels(x1, height - (pos.second - 1) , 1, 1, GL_RGBA, GL_FLOAT, pixel);
				if (!spanAbove && !(visited[x1 * height + (pos.second - 1)] || pixel[3])) {
					stack.push({ x1, pos.second - 1 });
					spanAbove = true;
				}
				else if (spanAbove && (visited[x1 * height + (pos.second - 1)] || pixel[3])) {
					spanAbove = false;
				}
			}

			if (pos.second < height - 1) {
				glReadPixels(x1, height - (pos.second + 1), 1, 1, GL_RGBA, GL_FLOAT, pixel);
				if (!spanBelow && !(visited[x1 * height + (pos.second + 1)] || pixel[3])) {
					stack.push({ x1, pos.second + 1 });
					spanBelow = true;
				}
				else if (spanBelow && (visited[x1 * height + (pos.second + 1)] || pixel[3])) {
					spanBelow = false;
				}
			}			
		}
	}

	if (!fillPositions.empty()) {
		GLuint vertArray;
		glCreateVertexArrays(1, &vertArray);
		glBindVertexArray(vertArray);
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glNamedBufferStorage(buffer, sizeof(GLint) * fillPositions.size(), fillPositions.data(), 0);

		glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(GLint) * 2, 0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_POINTS, 0, fillPositions.size() / 2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}