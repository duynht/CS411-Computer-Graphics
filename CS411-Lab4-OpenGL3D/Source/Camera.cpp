#include "Camera.h"

Camera::Camera(float fov, float aspect, float zNear, float zFar, const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up) {
	this->perspective = glm::perspective(fov, aspect, zNear, zFar);
	this->position = pos;
	this->forward = glm::normalize(forward - pos);
	this->up = up;
}

glm::mat4 Camera::GetViewProjection() const {
	return perspective * glm::lookAt(position, position + forward, up);
}

void Camera::SetViewFocus(const glm::vec3& viewFocus) {
	forward = glm::normalize(viewFocus - position);
}
