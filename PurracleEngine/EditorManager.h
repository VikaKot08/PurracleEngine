#pragma once
#include <string>
#include <glm/glm.hpp>

class Model;

class EditorManager
{
public:
	void ChangeMesh(Model* aModel, const std::string& meshPath);
	void ChangeTexture(Model* aModel, const std::string& texturePath);
	void UpdateTransform(Model* aModel, glm::vec3 positionVec, glm::vec3 rotationVec, glm::vec3 scaleVec);
	void UpdateTransformMatrix(Model* aModel, glm::mat4 aMatrix);

};

