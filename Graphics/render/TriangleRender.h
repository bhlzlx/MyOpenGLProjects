#pragma once
#include <graphics/ShaderOGL.h>
#include <graphics/TexOGL.h>
#include <graphics/BufferOGL.h>

namespace ph
{

	struct TriElement
	{
		vec3	vert;
		vec2	coord;
	};

	struct TriData
	{
		StaticVB *		vbo;
		StaticIB *		ibo;
		VertexArray *	vao;
		TexOGL*			texture;
		PhU32			nPoint;
		TriData()
		{
			vbo = nullptr;
			ibo = nullptr;
			vao = nullptr;
		}
		static TriData * Create( TriElement* _triPtr, PhU32 _nPoint, TexOGL* _texture )
		{
			if (_nPoint >= 3)
			{
				TriData * td = new TriData;
				std::vector<PhU32> indices;
				for (size_t i = 0; i < _nPoint; ++i)
					indices.push_back(i);
				td->vbo = StaticVB::New(_triPtr, _nPoint * sizeof(TriElement));
				td->ibo = StaticIB::New(indices.data(), indices.size() * sizeof(PhU32));
				static VertexArray::Layout  layouts[] = {
					{ 0, 3, GL_FLOAT, sizeof(float) * 5, 0 },
					{ 1, 2, GL_FLOAT, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3) },
					{0, 0, 0, 0, 0}
				};
				td->vao = VertexArray::New(td->vbo, td->ibo, &layouts[0]);
				td->nPoint = _nPoint;
				td->texture = _texture;
				return td;
			}
			return nullptr;
		}
	};
	class TriangleRender
	{
	private:
		ShaderOGL * shader;
		// EffectOGL * effect; 背面剃除之类的一些渲染设置 ，目前还没加
	public:
		TriangleRender();
		~TriangleRender();
		void begin()
		{
			if (!shader)
			{
				return;
			}
			shader->Bind();
		}

		void end()
		{

		}

		void draw( const TriData* _data )
		{
			_data->vao->Bind();
			shader->BindTexture2D("PrimTexSampler");
			glDrawElements(GL_TRIANGLES, _data->nPoint, GL_UNSIGNED_INT, NULL);
		}

		static TriangleRender* GetInstance()
		{
			static TriangleRender tr;
			if (!tr.shader)
			{
				Archive * arch = GetDefArchive();
				IBlob * vertBlob = arch->Open("triangle.vert");
				IBlob * fragBlob = arch->Open("triangle.frag");
				tr.shader = ShaderOGL::CreateShader(vertBlob->GetBuffer(), fragBlob->GetBuffer());
				vertBlob->Release();
				fragBlob->Release();
			}
			return &tr;
		}
	};
}



