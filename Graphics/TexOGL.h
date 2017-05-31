#ifndef __PX_TEX_OGL_H__
#define __PX_TEX_OGL_H__

#include "ShaderOGL.h"
#include "GraphicDesc.h"
#include <PhBase/PhBase.h>

namespace ph
{
	GLenum TexFilter2GL( TEX_FILTER filter );
	GLenum TexAddress2GL( TEX_ADDRESS addr );
    struct Image
    {
        uint16_t    nWidth;
        uint16_t    nHeight;
        uint8_t *   pData;
        void Release();
		static Image * ImageWithPNG( IBlob * );
    };
	
	class SamplerOGL
	{
	private:
		GLuint			sampler;
		SamplerState	desc;
	public:
		SamplerOGL()
		{
			__gl_check_error__
			glGenSamplers( 1, &sampler );
			__gl_check_error__
			glSamplerParameteri( sampler, GL_TEXTURE_MIN_FILTER, TexFilter2GL( desc.MinFilter)); __gl_check_error__
			glSamplerParameteri( sampler, GL_TEXTURE_MAG_FILTER, TexFilter2GL(desc.MagFilter)); __gl_check_error__
			glSamplerParameteri( sampler, GL_TEXTURE_WRAP_S, TexAddress2GL(desc.AddressU)); __gl_check_error__
			glSamplerParameteri( sampler, GL_TEXTURE_WRAP_T, TexAddress2GL(desc.AddressV)); __gl_check_error__
			__gl_check_error__
		}
		
		void Bind( GLint _slot )
		{
			glBindSampler( _slot, sampler );
			__gl_check_error__
		}
	
		~SamplerOGL()
		{
			glDeleteSamplers(1, &sampler );
			__gl_check_error__
		}
		
		void SetDesc( SamplerState & _state )
		{
			this->desc = _state;
			glSamplerParameteri( sampler, GL_TEXTURE_MIN_FILTER, TexFilter2GL(_state.MinFilter));
			glSamplerParameteri( sampler, GL_TEXTURE_MAG_FILTER, TexFilter2GL(_state.MagFilter));
			glSamplerParameteri( sampler, GL_TEXTURE_WRAP_S, TexAddress2GL(_state.AddressU));
			glSamplerParameteri( sampler, GL_TEXTURE_WRAP_T, TexAddress2GL(_state.AddressV));
			__gl_check_error__
		}
	};

    class TexOGL
    {
	public:
        GLuint 					tex;
		PxTexDesc				desc;
		SamplerOGL				sampler;
	public:
        static TexOGL * CreateTexFromPNG( const char * _image );
        static TexOGL * CreateTexFromPNG( const Image * _image );
		static TexOGL * CreateTexEmpty( PIXEL_FORMAT _format, PhU16 _width, PhU16 _height );
		static TexOGL * CreateTexAlpha8(PhU16 _width, PhU16 _height);
		static TexOGL * CreateTexDepthStencil(PhU16 _width, PhU16 _height);
		static TexOGL * CreateTexStencil(PhU16 _width, PhU16 _height);
        static TexOGL * CreateWhiteTex();
	public:
		const PxTexDesc * GetDesc();
		void SetSampler( SamplerState & _sampler );
		bool Bind( ShaderOGL * _shader, const char * _name );
        void BufferSubData( int _x, int _y, int _w, int _h, unsigned char * _data );
        void GenMipmap();
        void Release();
    };

	typedef std::shared_ptr<TexOGL> TexOGLRef;
}

#endif // __PX_TEX_H__
