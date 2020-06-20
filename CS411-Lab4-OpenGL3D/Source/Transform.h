#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Transform {
	glm::vec3 translate;
	glm::vec3 negTranslate;
	glm::vec3 rotate;
	glm::vec3 scale;

public:
	Transform(const glm::vec3& translate = glm::vec3(), const glm::vec3& negTranslate = glm::vec3(0), const glm::vec3& rotate = glm::vec3(), const glm::vec3& scale = glm::vec3(1));

	glm::vec3& GetTranslate();
	glm::vec3& GetNegTranslate();
	glm::vec3& GetRotate();
	glm::vec3& GetScale();

	glm::mat4 GetMatrix() const;

	void SetTranslate(glm::vec3& translate);
	void SetNegTranslate(glm::vec3& negTranslate);
	void SetRotate(glm::vec3& rotate);
	void SetScale(glm::vec3& scale);
};