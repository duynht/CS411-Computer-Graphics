#pragma once
#include <fstream>

#include <glm/glm.hpp>
#include "GLCanvas.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Transform.h"
#include "Camera.h"
#include "Shape.h"
#include "Circle.h"
#include "Cube.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Torus.h"
#include "CircularHyperboloid.h"
#include "CircularParaboloid.h"

// TODO: Draw Shapes
//		- Circular Paraboloid
//		- Circular Hyperboloid

int main() {
	GLCanvas canvas(800, 800);
	Shader shader("Source/resources/shaders/BasicShaders");
	Texture texture("Source/resources/texture/bricks.jpg");

	std::vector<Shape*> shapes;

	shapes.push_back(new Cube(-2.f, 0.2f, -1.5f, 0.4f));
	shapes.push_back(new Sphere(-1.1f, 0.4f, -1.5f, 0.4f, 36, 18));
	shapes.push_back(new Cylinder(0.f, 0.5f, -1.5f, 0.4f, 1, 36));
	shapes.push_back(new Cone(1.f, 0.25f, -1.5f, 0.4f, 1, 36));
	shapes.push_back(new Circle(-1.5f, 0.f, 1.5f, 0.3f, 360));
	shapes.push_back(new Torus(-.5f, 0.f, 1.5f, 0.3f, 0.11f, 120, 360));
	shapes.push_back(new CircularHyperboloid(0.5f, 0.3f, 1.5f, 0.1f, 0.1f, 360, 18));
	shapes.push_back(new CircularParaboloid(1.5f, 0.5f, 1.5f, 0.5f, 0.5f, 360, 18));
	float radian = 0.f;

	while (!canvas.ShouldClose()) {
		canvas.Clear();
		Camera camera(70.0f, (float)canvas.GetWidth() / (float)canvas.GetHeight(), 0.01f, 1000.0f, glm::vec3(0.f, 2.f, 5.f));

		shader.Bind();
		//glm::mat4 mvp = glm::ortho(0.f, (float)canvas.GetWidth(), (float)canvas.GetHeight(), 0.f);

		glm::mat4 mvp = camera.GetViewProjection();
		shader.SetUniformMat4f("u_MVP", mvp);
		texture.Bind();

		for (auto shape : shapes) {
			shape->Rotate(glm::vec3(0.f, radian, 0.f));
			shape->Draw(shader);
		}

		canvas.Paint();
		radian -= 0.01f;
	}

	for (auto shape : shapes) {
		delete shape;
	}
	
	return 0;
}