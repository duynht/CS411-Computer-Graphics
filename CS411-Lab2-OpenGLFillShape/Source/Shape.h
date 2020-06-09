#pragma once
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <stack>
#include <math.h>
//#include <GL/gl3w.h>
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum class CGType {
	NONE = -1,
	CIRCLE = 0,
	ELLIPSE = 1,
	RECTANGLE = 2,
	POLYGON = 3,
	PARABOLA = 4,
	HYPERBOLA = 5,
	DDA_LINE = 6,
	BRES_LINE = 7,
};

class Shape {
protected:
	GLenum glPrimitive = GL_POINTS;
	GLint numPoints = 0;
	std::vector<GLint> positions;
	std::vector<GLint> fillPositions;
public:
	CGType type = CGType::NONE;
	virtual void draw();
	virtual void glDraw() {};
	virtual void fill(const GLint &seedX, const GLint &seedY, const GLint &width, const GLint &height);
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
	MidpointCircle(GLint xO, GLint yO, GLint xA, GLint yA);
	void draw();
};

class MidpointEllipse : public Shape {
	GLint xCenter, yCenter;
	GLuint rx, ry;
	double angle = 0.0;
public:
	MidpointEllipse(GLint xCenter, GLint yCenter, GLint a, GLint b);
	MidpointEllipse(GLint xF0, GLint yF0, GLint xF1, GLint yF1, GLint xE, GLint yE);
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

class CGRectangle : public Shape {
	GLint x[4];
	GLint y[4];
public:
	CGRectangle(GLint xUL, GLint yUL, GLint xBR, GLint yBR);
	void draw();
};

class CGPolygon : public Shape {
	std::vector<GLint> x;
	std::vector<GLint> y;
public:
	CGPolygon(std::vector<GLint> x, std::vector<GLint> y);
	void draw();
};