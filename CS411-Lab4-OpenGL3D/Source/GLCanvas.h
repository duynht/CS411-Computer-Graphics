#pragma once
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>
class GLCanvas {
	int width;
	int height;
	GLFWwindow* window;

public:
	GLCanvas(const int& w, const int& h);

	int ShouldClose();
	void Clear();
	void Paint();
	int GetWidth();
	int GetHeight();

	virtual ~GLCanvas();
};