#pragma once

#include <PhBase/PhBase.h>
#include <render/Glypher.h>
#include <graphics/RenderStateGL.h>
#include <BufferOGL.h>
#include <TexOGL.h>
#include <res/ObjModelLoader.h>
#include <glm/glm.hpp>

namespace ph
{

	struct Basic3DVertex
	{
		vec3	pos;
		vec3	norm;
		vec2	cood;
	};

	class Basic3DRender
	{
	private:
		ShaderOGL *		shader;
		//
		SamplerSlotRef	diffuseTexSlot;

		RenderStateGL   renderState;

		UniformBufferObjectRef transUBO;

		glm::mat4x4		matProj;
		glm::mat4x4		matView;
	public:

		bool Init(Archive * _arch)
		{
			IBlob * vert = _arch->Open("Basic3D.vert");
			IBlob * frag = _arch->Open("Basic3D.frag");
			shader = ShaderOGL::CreateShader(vert->GetBuffer(), frag->GetBuffer());
			if (!shader)
			{
				return false;
			}
			vert->Release();
			frag->Release();
			diffuseTexSlot = shader->GetSamplerSlot("diffuse_texture");
			ph::SamplerState ss;
			ss.MagFilter = ph::TEX_FILTER_POINT;
			ss.MinFilter = ph::TEX_FILTER_POINT;
			this->diffuseTexSlot->SetSamplerState(ss);

			transUBO = shader->AllocUniformBufferObject("RenderParam", 0);

			RenderState rs;
			rs.cullMode = CULL_MODE_NONE;
			rs.depthTestable = true;
			renderState = RenderStateGL(rs);

			return true;
		}

		bool Begin(const glm::mat4x4& _matView, const glm::mat4x4& _matProj)
		{
			matProj = _matProj;
			matView = _matView;
			// 
			shader->Bind();
			//
			renderState.Apply();

			transUBO->Bind();
			//
			transUBO->WriteData(&matView, sizeof(glm::mat4x4), sizeof(glm::mat4x4));
			transUBO->WriteData(&matProj, sizeof(glm::mat4x4) * 2, sizeof(glm::mat4x4));

			return true;
		}

		void Draw(const glm::mat4x4& _matModel, const ph::Model3D& _theMesh )
		{
			transUBO->WriteData(&_matModel, 0, sizeof(glm::mat4x4));
			for (auto & mesh : _theMesh.vecMesh)
			{
				mesh.vao->Bind();
				this->diffuseTexSlot->BindTexture( _theMesh.vecMaterial[mesh.material].texDiffuse.get() );
				transUBO->WriteData(&_theMesh.vecMaterial[mesh.material].diffuse, sizeof(glm::mat4x4) * 3, sizeof(float) * 3);
				//this->diffuseTexSlot->BindTexture( _theMesh.vecMaterial[mesh.material].texDiffuse.get() );
				glDrawElements(GL_TRIANGLES, mesh.nElement, GL_UNSIGNED_INT, 0 );
			}
		}

		void End()
		{

		}

		Basic3DRender() {}
		~Basic3DRender() {}
	};
}



