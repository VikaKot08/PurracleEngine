#include "Renderable.h"

glm::mat4 Renderable::GetMatrix() const
{
	glm::mat4 sceneMatrix = glm::translate(glm::mat4(1.0f), position);
	sceneMatrix = glm::rotate(sceneMatrix, rotation.x, glm::vec3(1, 0, 0));
	sceneMatrix = glm::rotate(sceneMatrix, rotation.y, glm::vec3(0, 1, 0));
	sceneMatrix = glm::rotate(sceneMatrix, rotation.z, glm::vec3(0, 0, 1));
	sceneMatrix = glm::scale(sceneMatrix, scale);

	glm::mat4 worldMatrix = sceneMatrix * localTransform;

	if(parent)
	{
		worldMatrix = parent->GetMatrix() * worldMatrix;
	}

	return worldMatrix;
}

void Renderable::SetMatrix(const glm::mat4& matrix)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::quat rotationQuat;
	glm::vec3 scaleVec;
	glm::vec3 translation;

	if(glm::decompose(matrix, scaleVec, rotationQuat, translation, skew, perspective))
	{
		position = translation;
		scale = scaleVec;
		rotation = glm::eulerAngles(rotationQuat);
	}
}
