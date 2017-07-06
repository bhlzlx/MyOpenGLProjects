#include "GUIRender.h"
#include "UIWidget.h"

namespace ph
{
	GUIRender::GUIRender()
	{
		shader = nullptr;
		RenderState rs;
		rs.blendDest = BLEND_FACTOR_INVSRCALPHA;
		rs.blendSrc = BLEND_FACTOR_SRCALPHA;
		rs.blendable = true;
		renderState = RenderStateGL( rs );
	}
	bool GUIRender::Init(Archive * _arch)
	{
		vertexArray = std::shared_ptr<UIVertexArray>(new UIVertexArray, [&](UIVertexArray * ptr)
		{
			ptr->Release();
		});
		vertexArray->Init();
		// 
		glypher.Init("MinecraftAE.ttf");
		//glypher.Init("font.ttf");
		// shader
		IBlob * vert = _arch->Open("GUIRender.vert");
		IBlob * frag = _arch->Open("GUIRender.frag");
		shader = ShaderOGL::CreateShader(vert->GetBuffer(), frag->GetBuffer());
		vert->Release();
		frag->Release();
		baseTexSlot = shader->GetSamplerSlot("PrimTexSampler");
		maskTexSlot = shader->GetSamplerSlot("MaskTexSampler");

		ph::SamplerState ss;
		ss.MagFilter = ph::TEX_FILTER_POINT;
		ss.MinFilter = ph::TEX_FILTER_POINT;
		this->baseTexSlot->SetSamplerState(ss);
		this->maskTexSlot->SetSamplerState(ss);		

		if (!shader)
		{
			return false;
		}
		this->width = 1366;
		this->height = 768;
		return true;
	}
	GUIRender::~GUIRender()
	{
		if (shader)
			shader->Release();
	}

	void GUIRender::Begin()
	{
		renderState.Apply(); __gl_check_error__
		shader->Bind(); __gl_check_error__
		vertexArray->Bind();
	}

	void GUIRender::End()
	{

	}

	UIVertexArray * GUIRender::GetVertexPool()
	{
		return this->vertexArray.get();
	}

	TexOGLRef GUIRender::GetGlypherTex( size_t _index )
	{
		return glypher.GetCanvasTexRef( _index );
	}
	
	GUIRender * GUIRender::GetInstance(Archive * _arch )
	{
		static GUIRender * pRender = nullptr;
		if (!pRender)
		{
			if (!_arch)
				return nullptr;
			pRender = new GUIRender();
			if (pRender->Init(_arch))
			{
				return pRender;
			}
			return nullptr;
		}
		return pRender;
	}

	void GUIRender::GUIViewport(PhU32 _width, PhU32 _height)
	{
		GUIRender * render = GetInstance();
		if (render)
		{
			render->width = _width;
			render->height = _height;
		}
	}

	void GUIRender::Draw(UIWidget * _widget)
	{
		for (const auto& it : _widget->vecRenderTargets)
		{
			baseTexSlot->BindTexture( it.texture.get() );
			maskTexSlot->BindTexture( it.mask.get() );
			shader->SetUniform("Color",1, (void*)(&it.color));
			shader->SetUniform("Gray", 1, (void*)(&it.gray));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(PhU32)* it.vertId * 6));
		}
	}
}


