#include "VertexArray.h"

#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind(); // we need to bind the VAO before we bind the VertexBuffer

	vb.Bind();
	
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		// https://docs.gl/gl3/glEnableVertexAttribArray
		GLCall(glEnableVertexAttribArray(i));

		// https://docs.gl/gl3/glVertexAttribPointer
		GLCall(glVertexAttribPointer(i, element.count, element.type, 
			element.isNormalized, layout.GetStride(), (const void *)offset));

		offset += element.count * VertexBufferLayoutElement::GetSizeOfType(element.type);
	}

}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}