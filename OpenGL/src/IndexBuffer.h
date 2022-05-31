#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_count;
public:
	IndexBuffer(const unsigned int* data,  unsigned int count); //constructor with two arguments
	~IndexBuffer();

	void bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_count;  }

};