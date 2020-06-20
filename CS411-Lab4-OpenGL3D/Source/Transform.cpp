#include "Transform.h"

Transform::Transform(const glm::vec3& translate, const glm::vec3& negTranslate, const glm::vec3& rotate, const glm::vec3& scale) {
	this->translate = translate;
	this->negTranslate = negTranslate;
	this->rotate = rotate;
	this->scale = scale;
}

glm::vec3& Transform::GetTranslate() {
	return translate;
}

glm::vec3& Transform::GetRotate() {
	return rotate;
}

glm::vec3& Transform::GetScale()
{
	return scale;
}

glm::vec3& Transform::GetNegTranslate()
{
	return negTranslate;
}

glm::mat4 Transform::GetMatrix() const{
	glm::mat4 translateMatrix = glm::translate(translate);
	glm::mat4 negTranslateMatrix = glm::translate(negTranslate);
	glm::mat4 scaleMatrix = glm::scale(scale);

	glm::mat4 rotXMatrix = glm::rotate(rotate.x, glm::vec3(1, 0, 0));
	glm::mat4 rotYMatrix = glm::rotate(rotate.y, glm::vec3(0, 1, 0));
	glm::mat4 rotZMatrix = glm::rotate(rotate.z, glm::vec3(0, 0, 1));
	glm::mat4 rotMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

	return translateMatrix * rotMatrix * negTranslateMatrix * scaleMatrix;
}

void Transform::SetTranslate(glm::vec3& translate) {
	this->translate = translate;
}

void Transform::SetNegTranslate(glm::vec3& negTranslate) {
	this->negTranslate = negTranslate;
}

void Transform::SetRotate(glm::vec3& rotate) {
	this->rotate = rotate;
}

void Transform::SetScale(glm::vec3& scale) {
	this->scale = scale;
}
