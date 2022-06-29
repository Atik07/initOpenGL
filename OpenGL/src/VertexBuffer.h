#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID; 
	// this ID would be the unique identifier that OpenGL provides the object
public:
	// data - vertex data; size - in bytes;
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};