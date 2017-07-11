#include "BufferOGL.h"
#include <PhBase/PhBase.h>

namespace ph
{
	std::shared_ptr<StaticVBO> StaticVBO::New(const  void* _data, size_t _size)
	{
		StaticVBORef ref( new StaticVBO() );
		glGenBuffers(1, &ref->buffer);
		__gl_check_error__
			glBindBuffer(GL_ARRAY_BUFFER, ref->buffer);
		__gl_check_error__
			glBufferData(GL_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
		__gl_check_error__
		ref->size = _size;
		return ref;
	}

	void StaticVBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
	}

	StaticVBO::~StaticVBO()
	{
		glDeleteBuffers(1, &buffer);
		__gl_check_error__
			buffer = 0;
		size = 0;
	}

	StaticIBORef StaticIBO::New(const  void * _data, size_t _size)
	{
		StaticIBORef ref( new StaticIBO() );
		glGenBuffers(1, &ref->buffer);
		__gl_check_error__
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ref->buffer);
		__gl_check_error__
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _data, GL_STATIC_DRAW);
		__gl_check_error__
		ref->size = _size;
		return ref;
	}

	void StaticIBO::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	}

	StaticIBO::~StaticIBO()
	{
		glDeleteBuffers(1, &buffer);
		__gl_check_error__
			buffer = 0;
		size = 0;
	}

	std::shared_ptr<VertexArray> VertexArray::New(IBindable * _vb, IBindable * _ib, VertexArray::Layout* _layout)
	{
		VertexArrayRef ref( new VertexArray );
		glGenVertexArrays(1, &ref->va);	__gl_check_error__
		ref->ib = _ib;
		ref->vb = _vb;
		VertexArray::Layout * layout = _layout;
		ref->layouts = _layout;
		ref->nlayout = 0;
		while (layout->size != 0)
		{
			++ref->nlayout;
			++layout;
		}
		return ref;
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

	VertexArray::~VertexArray()
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &va);
	}


	DynamicVBORef DynamicVBO::New( size_t _size )
	{
		DynamicVBORef ref(new DynamicVBO());
		glGenBuffers(1, &ref->buffer); __gl_check_error__
		glBindBuffer(GL_ARRAY_BUFFER, ref->buffer); __gl_check_error__
		glBufferData(GL_ARRAY_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW); __gl_check_error__
		ref->size = _size;

		return ref;
	}

	bool DynamicVBO::BufferData( const void * _data, size_t _offset, size_t _size)
	{
		if (size >= _offset + _size)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer); __gl_check_error__
			glBufferSubData(GL_ARRAY_BUFFER, _offset, _size, _data); __gl_check_error__
			return true;
		}
		return false;
	}

	bool DynamicVBO::Resize(size_t _size)
	{
		if (_size > size)
		{
			// 创建新的buffer对象
			GLuint newBuffer;
			glGenBuffers(1, &newBuffer); __gl_check_error__
			// 设置新的对象为写入对象
			glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer); __gl_check_error__
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			// 设置buffer大小
			glBufferData(GL_COPY_WRITE_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW); __gl_check_error__
			// 设置当前buffer为读取源
			glBindBuffer(GL_COPY_READ_BUFFER, buffer); __gl_check_error__
			// 复制数据
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size); __gl_check_error__
			// 删除旧buffer
			glDeleteBuffers(1, &this->buffer); __gl_check_error__
			// 
			this->buffer = newBuffer;
			this->size = _size;
			return true;
		}
		return false;
	}
	// dtor

	size_t DynamicVBO::Size()
	{
		return size;
	}

	inline DynamicVBO::~DynamicVBO()
	{
		glDeleteBuffers(1, &buffer);
	}

	void DynamicVBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
	}

	std::shared_ptr<DynamicIBO> DynamicIBO::New(size_t _size)
	{
		std::shared_ptr< DynamicIBO > ref(new DynamicIBO());
		glGenBuffers(1, &ref->buffer); __gl_check_error__;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ref->buffer); __gl_check_error__;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, nullptr, GL_STATIC_DRAW); __gl_check_error__;
		ref->size = _size;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return ref;
	}

	void DynamicIBO::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	}

	bool DynamicIBO::BufferData(const void * _data, size_t _offset, size_t _size)
	{
		if (_offset + _size <= this->size)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); __gl_check_error__;
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _offset, _size, _data); __gl_check_error__;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); __gl_check_error__;
			return true;
		}
		return false;
	}

	bool DynamicIBO::Resize(size_t _size)
	{
		if (_size > size)
		{
			// 创建新的buffer对象
			GLuint newBuffer;
			glGenBuffers(1, &newBuffer); __gl_check_error__;
			// 设置新的对象为写入对象
			glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer); __gl_check_error__;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
			// 设置buffer大小
			glBufferData(GL_COPY_WRITE_BUFFER, _size, nullptr, GL_DYNAMIC_DRAW); __gl_check_error__;
			// 设置当前buffer为读取源
			glBindBuffer(GL_COPY_READ_BUFFER, buffer); __gl_check_error__;
			// 复制数据
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size); __gl_check_error__;
			// 删除旧buffer
			glDeleteBuffers(1, &this->buffer); __gl_check_error__;
			// 
			this->buffer = newBuffer;
			this->size = _size;
			return true;
		}
		return false;
	}

	/*inline bool DynamicIBO::Resize(size_t _size)
	{
		if (_size > this->size)
		{
			GLuint newBuffer;
			__gl_check_error__
				glBindBuffer(GL_COPY_READ_BUFFER, buffer); __gl_check_error__
				glGenBuffers(1, &newBuffer); __gl_check_error__
				glBindBuffer(GL_COPY_WRITE_BUFFER, newBuffer); __gl_check_error__
				glBufferData(GL_COPY_WRITE_BUFFER, _size, nullptr, GL_STATIC_DRAW); __gl_check_error__

				glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->size); __gl_check_error__
				glDeleteBuffers(1, &buffer); __gl_check_error__
				this->buffer = newBuffer;
			this->size = _size;
		}
		return true;
	}*/

	size_t DynamicIBO::Size()
	{
		return size;
	}

	inline DynamicIBO::~DynamicIBO()
	{
		glDeleteBuffers(1, &buffer);
		buffer = 0;
		size = 0;
	}


}