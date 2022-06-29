#include "Texture.h"

#include "vendor/stb_image/stb_image.h" // may set in include path for the compiler

Texture::Texture(const std::string& filepath)
	: m_RendererID(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	/* we need to flib the image vertically .. 
	because OpenGL reads it 0,0 on bottom-left .. NOT top-left*/
	stbi_set_flip_vertically_on_load(1);
	// passing the parameters by reference so that STB writes the value to our member variables
	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4); 

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	
	/* setting parameters for our generated texture here "STUDY THIS!!!" */
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	/* THESE 4 parameter we need to set .. otherwise we will get a black texture*/

	// https://docs.gl/gl3/glTexImage2D
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	/* Also INTERNAL FORMAT vs FORMAT ..esentially ..
	first is how data is stored ..and second is how it should be read 
	or something like that
	*/

	// unbinding our texture
	GLCall(glBindTexture(GL_TEXTURE_2D,0));

	// deleting the pixel data from the CPU
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	// deleting the texture from the GPU
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot /*= 0*/) const
{
	// specifying a texture slot
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));

	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}