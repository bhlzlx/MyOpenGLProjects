#pragma once
#include <graphics/BufferOGL.h>
#include <graphics/ShaderOGL.h>
#include <PhBase/PhBase.h>
#include <render/Glypher.h>
#include <graphics/RenderStateGL.h>

namespace ph
{
	struct UIWidget;
	struct UIVertexArray;

	class GUIRender
	{
	private:
		friend struct UIWidget;
		// buffer object
		std::shared_ptr< UIVertexArray >	vertexArray;
		// shader
		ShaderOGL *		shader;
		RenderStateGL   renderState;
		Glypher			glypher;
		// 
		float			width;
		float			height;
	public:
		GUIRender();
		~GUIRender();

		bool Init(Archive * _arch);

		void Begin();

		void Draw(UIWidget * _widget);

		void End();

		UIVertexArray * GetVertexPool();
		TexOGLRef GetGlypherTex( size_t _index );

		static GUIRender* GetInstance(Archive * _arch = nullptr);
		static void GUIViewport( PhU32 _width, PhU32 _height );
	};

}
