#ifndef __PX_SHADER_OGL_H__
#define __PX_SHADER_OGL_H__

#include <PhBase/PhBase.h>
#include <vector>
#include <gl/gl3w.h>
#include <assert.h>
#include "GraphicDesc.h"
#include "BufferOGL.h"

namespace ph
{
	inline GLenum TexFilter2GL(TEX_FILTER filter)
	{
		switch (filter)
		{
		case TEX_FILTER_POINT: return GL_NEAREST;
		case TEX_FILTER_LINEAR: return GL_LINEAR;
		case TEX_FILTER_MIP_POINT: return GL_NEAREST_MIPMAP_NEAREST;
		case TEX_FILTER_MIP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
		default: return GL_NEAREST;
		}
	}

	inline GLenum TexAddress2GL(TEX_ADDRESS addr)
	{
		switch (addr)
		{
		case TEX_ADDRESS_REPEAT: return GL_REPEAT;
		case TEX_ADDRESS_CLAMP: return GL_CLAMP_TO_EDGE;
		case TEX_ADDRESS_MIRROR: return GL_MIRRORED_REPEAT;
		default: return GL_REPEAT;
		}
	}

	class TexOGL;
	class ShaderOGL;
	/* Sampler 在 GLSL 中的描述句柄 */
	class SamplerSlot
	{
		friend class ShaderOGL;
	private:
		std::string		slotName;		// 名字
		GLuint			uniformLoc;		// glsl中 sampler的 uniform location
		GLuint			slot;			// 纹理绑定槽
		SamplerState	samplerState;	// 纹理采样状态
	public:
		SamplerSlot(const char * _name, GLuint _loc, GLuint _slot);
		void BindTexture( TexOGL* _texture);
		void SetSamplerState( const SamplerState& _samplerState );
		void ApplySamplerState();
	};

	typedef std::shared_ptr< SamplerSlot > SamplerSlotRef;

    class ShaderOGL
    {
    public:
        ShaderOGL();
        ~ShaderOGL();
		
		struct uniform_item
		{
			GLint		id;
			GLenum 		type;
			GLint		size;
			char		name[32];
			GLint 		index; 
		};
        
        void Bind();
        void Release();
    private:
        GLuint      m_vert;
        GLuint      m_frag;
        GLuint      m_prog;
		std::vector< uniform_item >						m_vecUniform;
		std::vector< std::shared_ptr<SamplerSlot> >		m_samplerSlots;
    public:
        static ShaderOGL * CreateShader( const char * _vert, const char * _frag );
	public:
		bool SetUniform( const char * _name, int _count, void * _data );
		GLuint BindTexture2D( const char * _uniform );
		SamplerSlotRef GetSamplerSlot( const char * _name );
		UniformBufferObjectRef AllocUniformBufferObject(const char * _name, size_t _bindSlotID );
	};
}

#endif // __PX_SHADER_OGL_H__
