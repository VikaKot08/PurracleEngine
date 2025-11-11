#pragma once
class Texture
{
public:
	Texture(const char* aPath);
	~Texture();

	int Width;
	int Height;

	unsigned int TextureObject;

	void Bind(unsigned int slot) const;
	void Unbind();
};

