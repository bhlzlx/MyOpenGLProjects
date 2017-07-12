#pragma once
#include <gl/gl3w.h>
#include <gl/gl.h>
#include <PhBase/PhBase.h>
#include <assert.h>

namespace ph
{
	class IBindable
	{
	public:
		virtual void Bind() = 0;
	};

	class IWritableBuffer
	{
	public:
		virtual bool BufferData(const void * _data, size_t _offset, size_t _size) = 0;
	};

	class IMutableBuffer
	{
	public:
		virtual bool AppendBuffer(const void * _data, size_t _size) = 0;
	};


	class StaticVBO : public IBindable
	{
	private:
		GLuint      buffer;
		PhSizeT     size;
	public:
		static std::shared_ptr< StaticVBO > New(const void * _data, PhSizeT _size);
		virtual void Bind();
		~StaticVBO();
	};

	class StaticIBO : public IBindable
	{
	private:
		GLuint      buffer;
		PhSizeT     size;
	public:
		static std::shared_ptr< StaticIBO > New(const void * _data, PhSizeT _size);
		void Bind();
		~StaticIBO();
	};

	typedef std::shared_ptr< StaticIBO > StaticIBORef;
	typedef std::shared_ptr< StaticVBO > StaticVBORef;

	class DynamicVBO :public IBindable, IWritableBuffer
	{
	private:
		GLuint		buffer;
		size_t		size;
	public:
		static std::shared_ptr< DynamicVBO > New( size_t _size );
		// IBinable
		void Bind();
		// IWritable
		bool BufferData( const void * _data, size_t _offset, size_t _size);
		// IMutableBuffer
		bool Resize( size_t _size );
		// dtor
		size_t Size();
		~DynamicVBO();
	};

	typedef std::shared_ptr< DynamicVBO > DynamicVBORef;

	class DynamicIBO:public IBindable, IWritableBuffer
	{
	private:
		GLuint buffer;
		size_t size;
	public:
		static std::shared_ptr< DynamicIBO > New(size_t _size);
		// IBinable
		void Bind();
		// IWritable
		bool BufferData(const void * _data, size_t _offset, size_t _size);
		//
		size_t Size();
		bool Resize( size_t _size );

		~DynamicIBO();
	};

	typedef std::shared_ptr< DynamicIBO > DynamicIBORef;

	class VertexArray:public IBindable
	{
	public:
		friend class std::shared_ptr<VertexArray>;
		struct Layout
		{
			GLuint      index;
			GLint       size;
			GLenum      type;
			GLsizei     stride;
			GLvoid*     offset;
		};
	public:
		GLuint          va;
		IBindable *     vb;
		IBindable *     ib;
		Layout *        layouts;
		unsigned short  nlayout;
	public:
		static std::shared_ptr<VertexArray> New(IBindable * _vb, IBindable * _ib, VertexArray::Layout* _layout);
		void Bind();
		~VertexArray();
	};

	typedef std::shared_ptr<VertexArray> VertexArrayRef;

	// Uniform Buffer Object  | UBO

	struct UniformBufferObject
	{
		GLuint blockIndex;
		GLuint bindSlotID;
		GLuint bufObj;
		GLuint bufSize;

		void Bind()
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, bindSlotID, bufObj);
		}

		void WriteData(const void * _data, size_t _offset, size_t _size)
		{
			assert(_offset + _size <= bufSize);
			glBindBuffer(GL_UNIFORM_BUFFER, bufObj);
			glBufferSubData(GL_UNIFORM_BUFFER, _offset, _size, _data);
		}

		UniformBufferObject()
		{
			blockIndex = bindSlotID = bufObj = bufSize = 0;
		}

		~UniformBufferObject()
		{
			glDeleteBuffers(1, &bufObj);
		}
	};

	typedef std::shared_ptr< UniformBufferObject > UniformBufferObjectRef;

}