#pragma once
#include <string>
#include <vector>
#include "Shader.h"
#include "Texture.h"
#include "Renderable.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

};

struct MeshTexture
{
	unsigned int TextureObject;
	std::string type;
	std::string path;
};

class Mesh : public Renderable
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indicies);
	~Mesh() override;
	void Render(Shader* myShader) override;
private:
	GLsizei indexCount;
};

