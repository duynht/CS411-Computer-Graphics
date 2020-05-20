#pragma once
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

enum class CGType {
	NONE = -1,
	DDA_LINE = 0,
	BRES_LINE = 1,
	CIRCLE = 2,
	ELLIPSE = 3,
	PARABOLA = 4,
	HYPERBOLA = 5
};

class Shape {
protected:
	GLuint glPrimitive = GL_POINTS;
	GLint numPoints = 0;
	std::vector<GLint> positions;
public:
	CGType type = CGType::NONE;
	virtual void draw();
	virtual void glDraw() {};
};

class BresenhamLine : public Shape {
	GLint x0, y0, x1, y1;
public:
	BresenhamLine(GLint xA, GLint yA, GLint xB, GLint yB);
	void draw();
	void glDraw();
};

class DDALine : public Shape {
	GLint x0, y0, x1, y1;
public:
	DDALine(GLint xA, GLint yA, GLint xB, GLint yB);
	void draw();
	void glDraw();
};

class MidpointCircle : public Shape {
	GLint xCenter, yCenter, radius;
public:
	MidpointCircle(GLint xCenter, GLint yCenter, GLint radius);
	void draw();
};

class MidpointEllipse : public Shape {
	GLint xCenter, yCenter, rx, ry;
public:
	MidpointEllipse(GLint xCenter, GLint yCenter, GLint a, GLint b);
	void draw();
};

class MidpointParabola : public Shape {
	GLint xFocus, yFocus, p, bound;
public:
	MidpointParabola(GLint xFocus, GLint yFocus, GLint focalLength, GLint bound);
	void draw();
};

class MidpointHyperbola : public Shape {
	GLint xCenter, yCenter, rx, ry, bound;
public:
	MidpointHyperbola(GLint xCenter, GLint yCenter, GLint a, GLint b, GLint yBound);
	void draw();
};

