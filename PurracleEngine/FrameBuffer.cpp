#include "FrameBuffer.h"
#include <iostream>


FrameBuffer::~FrameBuffer()
{
	if(colorTexture)
	{
		glDeleteTextures(1, &colorTexture);
	}

	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (depthBuffer)
	{
		glDeleteRenderbuffers(1, &depthBuffer);
	}
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::Unbind() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(int aWidth, int aHeight) 
{
	if (aWidth == width && aHeight == height) return;
	width = aWidth;
	height = aHeight;

	Destroy();
	Create();

}
void FrameBuffer::ValidSize(int aWidth, int aHeight) 
{
	if (width != aWidth || height != aHeight) Resize(aWidth, aHeight);
}

void FrameBuffer::Create() 
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Destroy() 
{
	if (colorTexture)
	{
		glDeleteTextures(1, &colorTexture);
		colorTexture = 0;
	}

	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
	}

	if (depthBuffer)
	{
		glDeleteRenderbuffers(1, &depthBuffer);
		depthBuffer = 0;
	}
}
