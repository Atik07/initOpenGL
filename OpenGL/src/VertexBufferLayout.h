#pragma once

#include <GLEW/glew.h>

#include "Renderer.h"

#include <vector>

/*
	This command is what defines the layout
	
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0));
	
	2 - is the count .. so our layout element would require a count member
	GL_FLOAT - type .. 
	GL_FALSE - state of normalisation ..

	sizeof(float) * 2 - stride
	(const void*) 0 - offset
*/

struct VertexBufferLayoutElement
{
	unsigned int type;
	unsigned int count;
	unsigned char isNormalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT: return 4;
			case GL_UNSIGNED_INT: return 4;
			case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferLayoutElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout()
		: m_Stride(0) {}
	// ~VertexBufferLayout();

	template<typename T>
	void Push(unsigned int count) 
	{
		static_assert(false);
		// static_assert tests assertions at compile time, find here: https://docs.microsoft.com/en-us/cpp/cpp/static-assert?view=msvc-170
	}

	// creating template specialisations
	template<>
	void Push<float>(unsigned int count) 
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * VertexBufferLayoutElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferLayoutElement> GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};