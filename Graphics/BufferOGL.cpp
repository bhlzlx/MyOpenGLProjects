#include "BufferOGL.h"
#include <PhBase/PhBase.h>

namespace ph
{
	StaticVB* StaticVB::New(const  void* _data, size_t _size)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		__gl_check_error__
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		__gl_check_error__
			glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
		__gl_check_error__
			StaticVB * vb = new StaticVB;
		vb->buffer = buffer;
		vb->size = _size;
		return vb;
	}

	void StaticVB::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
	}

	void StaticVB::Release()
	{
		glDeleteBuffers(1, &buffer);
		__gl_check_error__
			buffer = 0;
		size = 0;
		delete this;
	}

	StaticIB* StaticIB::New(const  void * _data, size_t _size)
	{
		GLuint buffer;
		glGenBuffers(1, &buffer);
		__gl_check_error__
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		__gl_check_error__
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
		__gl_check_error__
			StaticIB * ib = new StaticIB;
		ib->buffer = buffer;
		ib->size = _size;
		return ib;
	}

	void StaticIB::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	}

	void StaticIB::Release()
	{
		glDeleteBuffers(1, &buffer);
		__gl_check_error__
			buffer = 0;
		size = 0;
		delete this;
	}

	VertexArray * VertexArray::New(IBindable * _vb, IBindable * _ib, VertexArray::Layout* _layout)
	{
		VertexArray * _va = new VertexArray;
		glGenVertexArrays(1, &_va->va);	__gl_check_error__
		_va->ib = _ib;
		_va->vb = _vb;
		VertexArray::Layout * layout = _layout;
		_va->layouts = _layout;
		_va->nlayout = 0;
		while (layout->size != 0)
		{
			++_va->nlayout;
			++layout;
		}
		return _va;
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(va); __gl_check_error__
		vb->Bind(); __gl_check_error__
		ib->Bind(); __gl_check_error__
		for (int i = 0; i<nlayout; ++i)
		{
			glEnableVertexAttribArray(layouts[i].index); __gl_check_error__
			glVertexAttribPointer(layouts[i].index, layouts[i].size, layouts[i].type, GL_FALSE, layouts[i].stride, layouts[i].offset); __gl_check_error__
		}
	}

	void VertexArray::Release()
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &va);
		delete this;
	}



	DynamicVBO::DynamicVBO(size_t _nItem, size_t _typeSize)
	{
		typeSize = _typeSize;
		glGenBuffers(1, &buffer); __gl_check_error__
			glBindBuffer(GL_ARRAY_BUFFER, buffer); __gl_check_error__
			glBufferData(GL_ARRAY_BUFFER, typeSize * _nItem, nullptr, GL_DYNAMIC_DRAW); __gl_check_error__
			this->capacity = typeSize * _nItem;
		this->size = 0;
	}

	bool DynamicVBO::Write(size_t _index, const void* _data)
	{
		if (size >= typeSize * (_index + 1))
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer); __gl_check_error__
				glBufferSubData(GL_ARRAY_BUFFER, typeSize * _index, typeSize, _data); __gl_check_error__
				return true;
		}
		return false;
	}

	void DynamicVBO::PushBack(const void* _data)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		if (capacity - size <= typeSize)
		{
			size_t newCapacity = capacity * 2;
			GLuint newBuffer;

			glBindBuffer(GL_COPY_READ_BUFFER, buffer); __gl_check_error__
			glGenBuffers(1, &newBuffer); __gl_check_error__
			glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer); __gl_check_error__
			glBufferData(GL_COPY_WRITE_BUFFER, newCapacity, nullptr, GL_DYNAMIC_DRAW); __gl_check_error__

			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->size); __gl_check_error__
			glDeleteBuffers(1, &this->buffer); __gl_check_error__
			this->buffer = newBuffer;
			this->capacity = newCapacity;
		}
		glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
		glBufferSubData(GL_ARRAY_BUFFER, this->size, typeSize, _data); __gl_check_error__
			size += typeSize;
	}

	size_t DynamicVBO::ItemCount()
	{
		return this->size / typeSize;
	}

	size_t DynamicVBO::ItemCapacity()
	{
		return this->capacity / typeSize;
	}

	void DynamicVBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
	}

	void DynamicVBO::Release()
	{
		glDeleteBuffers(1, &buffer);
	}

}