#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FrameBuffer
{
public:
	FrameBuffer(int aWidth, int aHeight) : width(aWidth), height(aHeight) { Create(); }
	~FrameBuffer();

	void Bind();
	void Unbind();
	void Resize(int aWidth, int aHeight);
	void ValidSize(int aWidth, int aHeight);

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	GLuint GetTexture() const { return colorTexture; }

private:

	int width;
	int height;
	GLuint FBO = 0;
	GLuint colorTexture = 0;
	GLuint depthBuffer = 0;

	void Create();
	void Destroy();
};

