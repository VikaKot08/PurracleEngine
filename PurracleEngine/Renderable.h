#pragma once
#include "Shader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

class Renderable
{
public:
    virtual ~Renderable() = default;

    virtual void Render(Shader* myShader) = 0;
protected:
	unsigned int VBO = 0;
	unsigned int VAO = 0;
	unsigned int EBO = 0;
public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	std::string name = "NoName";

	glm::mat4 localTransform{ 1.0f };

	void SetLocalTransform(const glm::mat4& matrix) { localTransform = matrix; }
	void AddChild(Renderable* aChild);
	void RemoveChild(Renderable* aChild);
	void SetParent(Renderable* aParent);
	void RemoveParent() { parent = nullptr;}

	Renderable* parent = nullptr;
	std::vector<Renderable*> children;

	glm::mat4 GetMatrix() const;

	void SetMatrix(const glm::mat4& matrix);
};

