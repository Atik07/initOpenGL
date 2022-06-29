#pragma once

#include "ErrorHandling.h"
#include <string>

class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP; // BPP - bits per pixel
public:
	Texture(const std::string& filepath);
	~Texture();

	void Bind(unsigned int slot = 0) const; // by default a texture will bind to slot 0
	// OpenGL allows us to bind more than one textures .. on windows it probably gives us 32 slots

	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};