#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size); //constructor with two arguments
	~VertexBuffer();

	void bind() const;
	void Unbind() const;

};