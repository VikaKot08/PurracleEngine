#include "Texture.h"
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>

Texture::Texture(const char* aPath)
{
	int Channels = 0;
	Width = 0;
	Height = 0;

	unsigned char* data = stbi_load(aPath, &Width, &Height, &Channels, 0);

	glGenTextures(1, &TextureObject);
	glBindTexture(GL_TEXTURE_2D, TextureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(data)
	{
		GLenum format;
		if (Channels == 1)
			format = GL_RED;
		else if (Channels == 3)
			format = GL_RGB;
		else if (Channels == 4)
			format = GL_RGBA;
		else
			format = GL_RGB;

		glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
	{
		std::cout << "Could not load texture" << std::endl;
	}

	stbi_image_free(data);
}

void Texture::Bind(unsigned int slot = 0) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, TextureObject);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	if (TextureObject)
		glDeleteTextures(1, &TextureObject);
}
