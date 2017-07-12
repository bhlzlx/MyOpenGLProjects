#pragma once

#include <PhBase/PhBase.h>
#include <Graphics/TexOGL.h>
#include <Graphics/BufferOGL.h>
#include <Graphics/res/TexPool.h>

namespace ph
{
	typedef struct ObjVertData
	{
		vec3	pos;
		vec3	norm;
		vec2	tex;
		ObjVertData()
		{
			pos.x = pos.y = pos.z = 0.0f;
			norm.x = norm.y = norm.z = 0.0f;
			tex.x = tex.y = 0.0f;
		}
	} BasicVert;

	struct AABB
	{
		vec3 min;
		vec3 max;
	};

	struct Material
	{
		char			name[32];
		vec3			ambient; // 环境光
		vec3			diffuse; // 散射光
		vec3			specular; // 镜面反射颜色
		float			shiness; // 镜面反射指数
		TexOGLRef		texAmbient;
		TexOGLRef		texDiffuse;
		TexOGLRef		texHighlight;
		static Material Default()
		{
			Material mtl;
			strcpy(&mtl.name[0], "default");
			mtl.ambient = { 0.3f, 0.3f, 0.3f };
			mtl.diffuse = { 0.5f, 0.5f, 0.5f };
			mtl.specular = { 0.2f, 0.2f, 0.2f };
			mtl.shiness = 254;
			mtl.texDiffuse = mtl.texHighlight = mtl.texAmbient = TexPool::GetWhite();
			return mtl;
		}
	};

	struct ClientSideMesh
	{
		std::vector< ObjVertData >	vertices;
		std::vector< unsigned int>  indices;
		TexOGLRef					tex;
		std::string					mtl;
	};

	struct ServerSideMesh
	{
		StaticVBORef	vbo;
		StaticIBORef	ibo;
		VertexArrayRef	vao;
		AABB			aabb;
		size_t			nElement;
		size_t			material;
	};

	typedef std::vector< Material >			MaterialVector;
	typedef ClientSideMesh					ClientMesh;
	typedef ServerSideMesh					Mesh;
	typedef std::vector< ClientSideMesh >	ClientMeshVector;
	typedef std::vector< ServerSideMesh >	MeshVector;

	typedef struct __Model
	{
		MeshVector			meshes;
		MaterialVector		materials;
		AABB				aabb;
	} BasicModel;

	typedef std::shared_ptr< BasicModel > BasicModelRef;
}
