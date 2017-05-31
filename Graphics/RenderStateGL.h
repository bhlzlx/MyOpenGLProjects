#pragma once
#include <graphics/GraphicDesc.h>
#include <gl/gl3w.h>
namespace ph
{
#define GL_STATE_SETTER( value, macro ) {\
	if( value )\
	{\
		glEnable( macro );\
	}else{\
		glDisable(macro);\
	}\
}

	inline GLenum BlendState2GL(PhU32 _blend)
	{
		switch (_blend)
		{
		case BLEND_FACTOR_ZERO: return GL_ZERO;
		case BLEND_FACTOR_ONE: return GL_ONE;
		case BLEND_FACTOR_DESTALPHA: return GL_DST_ALPHA;
		case BLEND_FACTOR_DESTCOLOR: return GL_DST_COLOR;
		case BLEND_FACTOR_INVDESTALPHA: return GL_ONE_MINUS_DST_ALPHA;
		case BLEND_FACTOR_INVDESTCOLOR: return GL_ONE_MINUS_DST_COLOR;
		case BLEND_FACTOR_INVSRCALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case BLEND_FACTOR_INVSRCCOLOR: return GL_ONE_MINUS_SRC_COLOR;
		case BLEND_FACTOR_SRCALPHA: return GL_SRC_ALPHA;
		case BLEND_FACTOR_SRCCOLOR: return GL_SRC_COLOR;
		default:
			return GL_ONE;
		}
	}

	inline GLenum Cmpfunc2GL(PhU32 _func)
	{
		switch (_func)
		{
		case CMP_FUNC_GREATER: return GL_GREATER;
		case CMP_FUNC_GREATER_EQUAL: return GL_GEQUAL;
		case CMP_FUNC_EQUAL: return GL_EQUAL;
		case CMP_FUNC_NOT_EQUAL: return GL_NOTEQUAL;
		case CMP_FUNC_LESS_EQUAL: return GL_LEQUAL;
		case CMP_FUNC_LESS: return GL_LESS;
		case CMP_FUNC_ALWAYS: return GL_ALWAYS;
		case CMP_FUNC_NEVER: return GL_NEVER;
		default: return GL_GREATER;
		}
	}

	

	class RenderStateGL
	{
	public:
		RenderStateGL();
		RenderStateGL( RenderState _rs );
		~RenderStateGL();
	private:
		RenderState rs;
	public:
		void Apply()
		{
			GL_STATE_SETTER(rs.scissorable, GL_SCISSOR_TEST);

			if (rs.depthTestable)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(Cmpfunc2GL(rs.depthFunc));
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}

			switch (rs.cullMode)
			{
			case CULL_MODE_NONE:
				glDisable(GL_CULL_FACE); break;
			case CULL_MODE_FRONT:
				glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
			case CULL_MODE_BACK:
				glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
			}

			if (rs.blendable)
			{
				glEnable(GL_BLEND);
				glBlendFunc( BlendState2GL( rs.blendSrc), BlendState2GL( rs.blendDest));
			}
			else
			{
				glDisable(GL_BLEND);
			}			
		}

	};
}


