#pragma once
#include <PhBase/PhBase.h>
#include <Graphics/BufferOGL.h>
#include <Graphics/RenderStateGL.h>
#include <Graphics/ShaderOGL.h>
#include <glm/glm.hpp>
#include <vector>

namespace ph
{
	class GridRender
	{
	private:
		StaticVBORef				vbo;
		StaticIBORef				ibo;
		VertexArrayRef				vao;
		size_t						nElement;
		UniformBufferObjectRef		ubo;
		ShaderOGL*					shader;
		RenderStateGL				rs;
	public:
		GridRender();
		~GridRender();
		void Init( Archive* _arch, float _cellSz, int _nGrid )
		{
			static float gridSize = 0.5;
			// 生成VBO IBO VAO
			// 100 * 100
			std::vector< vec3 > verts;
			std::vector< unsigned int > indices;

			int idxMin = -_nGrid / 2;
			int idxMax = -idxMin;
			float rangeMin = idxMin * _cellSz;
			float rangeMax = idxMax * _cellSz;

			for (int i = idxMin; i <= idxMax; ++i)
			{
				verts.push_back({ i * _cellSz, 0.0f, rangeMin });
				verts.push_back({ i * _cellSz, 0.0f, rangeMax });
				indices.push_back( indices.size());
				indices.push_back( indices.size());
			}
			for (int i = idxMin; i <= idxMax; ++i)
			{
				verts.push_back({ rangeMin, 0.0f, i * _cellSz });
				verts.push_back({ rangeMax, 0.0f, i * _cellSz });
				indices.push_back(indices.size());
				indices.push_back(indices.size());
			}
			vbo = StaticVBO::New(verts.data(), verts.size() * sizeof(vec3));
			ibo = StaticIBO::New(indices.data(), indices.size() * sizeof(unsigned int));
			nElement = verts.size();
			static VertexArray::Layout layout[] =
			{
				{ 0, 3, GL_FLOAT, sizeof(vec3), 0 },
				{0}
			};
			vao = VertexArray::New(vbo.get(), ibo.get(), layout);
			// 生成shader
			IBlob * vert = _arch->Open("Grid.vert");
			IBlob * frag = _arch->Open("Grid.frag");
			shader = ShaderOGL::CreateShader(vert->GetBuffer(), frag->GetBuffer());
			// 生成UBO对象
			ubo = shader->AllocUniformBufferObject("RenderParam", 0);
			// 渲染状态（默认的）
			RenderState ors;
			rs = RenderStateGL(ors);
		}

		bool Begin( const glm::mat4x4& _matView, const glm::mat4x4& _matProj)
		{
			// 应用shader
			shader->Bind();
			// 应用ubo
			ubo->Bind();
			// 写入proj矩阵
			ubo->WriteData(&_matProj, 0,sizeof(glm::mat4x4));
			// 写入view矩阵
			ubo->WriteData(&_matView, sizeof(glm::mat4x4), sizeof(glm::mat4x4));
			// 应用渲染状态
			rs.Apply();
			return true;
		}

		void Draw()
		{
			vao->Bind();
			vec4 color(1.0, 0, 1.0, 1.0);
			ubo->WriteData(&color, sizeof(glm::mat4x4) * 2 , sizeof(color));
			glDrawElements(GL_LINES, nElement, GL_UNSIGNED_INT, 0);
		}

		void End()
		{

		}
	};
}



