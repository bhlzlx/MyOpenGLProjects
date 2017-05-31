#ifndef __PX_SHADER_OGL_H__
#define __PX_SHADER_OGL_H__

#include <PhBase/PhBase.h>
#include <vector>
#include <gl/gl3w.h>

namespace ph
{
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
		std::vector< uniform_item > m_vecUniform;
    public:
        static ShaderOGL * CreateShader( const char * _vert, const char * _frag );
	public:
		bool SetUniform( const char * _name, int _count, void * _data );
		GLuint BindTexture2D( const char * _uniform );
	};
}

#endif // __PX_SHADER_OGL_H__
