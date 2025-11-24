#pragma once
#include <string>

class Texture
{
public:
	Texture(const char* aPath);
	~Texture();

	int Width;
	int Height;
	std::string path;

	unsigned int TextureObject;

	void Bind(unsigned int slot) const;
	void Unbind();
};

