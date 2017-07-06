#pragma once
#include <gl/gl3w.h>
#include <gl/gl.h>
#include <PhBase/PhBase.h>

namespace ph
{

	class IBindable
	{
	public:
		virtual void Bind()
		{

		}
		
		virtual void Release()
		{

		}

		~IBindable()
		{

		}
	};

	class IBufferOGL:public IBindable
	{
	public:
		virtual void Bind() = 0;
		virtual void Release() = 0;
		virtual void BufferData(const  void * _data, PhSizeT _size) {
			throw "bad invokition!";
		}
		virtual ~IBufferOGL() {
		}
	};

	class StaticVB :public IBufferOGL
	{
	private:
		GLuint      buffer;
		PhSizeT     size;
	public:
		static StaticVB* New(const void * _data, PhSizeT _size);
		virtual void Bind();
		virtual void Release();
	};

	class StaticIB :public IBufferOGL
	{
	private:
		GLuint      buffer;
		PhSizeT     size;
	public:
		static StaticIB* New(const void * _data, PhSizeT _size);
		void Bind();
		void Release();
	};

	class dynamic_ibo : public IBindable
	{
	private:
		GLuint buffer;
		size_t size;
	public:
		dynamic_ibo(size_t _size)
		{
			glGenBuffers(1, &buffer); __gl_check_error__
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); __gl_check_error__
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, nullptr, GL_STATIC_DRAW); __gl_check_error__
				size = _size;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		~dynamic_ibo()
		{
			glDeleteBuffers(1, &buffer);
		}

		bool Write(size_t _offset, size_t _size, PhU32* _data)
		{
			if (_offset + _size  <= this->size)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); __gl_check_error__
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _offset, _size, _data); __gl_check_error__
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); __gl_check_error__
					return true;
			}
			return false;
		}

		bool Resize(size_t _size)
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
		}

		PhU32 ItemSize()
		{
			return size / sizeof(PhU32);
		}

		void Bind()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		}

		void Release()
		{
			glDeleteBuffers(1, &buffer);
		}
	};

	class DynamicVBO:public IBindable
	{
	private:
		size_t		typeSize;
		GLuint		buffer;
		size_t		size;
		size_t		capacity;
	public:
		DynamicVBO(size_t _nItem, size_t _typeSize );
		~DynamicVBO()
		{
			glDeleteBuffers(1, &buffer);
		}
		bool Write(size_t _index, const void* _data);
		void PushBack(const void *_data);
		size_t ItemCount();
		size_t ItemCapacity();
		void Bind();
		void Release();
	};

	class VertexArray
	{
	public:
		struct Layout
		{
			GLuint      index;
			GLint       size;
			GLenum      type;
			GLsizei     stride;
			GLvoid*     offset;
		};
	private:
		GLuint          va;
		IBindable *     vb;
		IBindable *     ib;
		Layout *        layouts;
		unsigned short  nlayout;
	public:
		static VertexArray * New(IBindable * _vb, IBindable * _ib, VertexArray::Layout* _layout);
		void Bind();
		void Release();
	};

}