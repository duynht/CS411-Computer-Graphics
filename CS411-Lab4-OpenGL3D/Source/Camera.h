#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
	glm::mat4 perspective;
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
public:
	Camera(float fov, float aspect, float zNear, float zFar,
			const glm::vec3& pos = glm::vec3(0.f, 0.f, -3.f),
			const glm::vec3& forward = glm::vec3(0.f, 0.f, 1.f),
			const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f));

	glm::mat4 GetViewProjection() const;
	void SetViewFocus(const glm::vec3& viewFocus);
};
