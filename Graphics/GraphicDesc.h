#pragma once
#include <PhBase/PhBase.h>

namespace ph
{

	enum DRAW_TYPE
	{
		DRAW_TYPE_WIREDFRAME,
		DRAW_TYPE_TRINGLE
	};

	enum CULL_MODE
	{
		CULL_MODE_NONE,
		CULL_MODE_FRONT,
		CULL_MODE_BACK,
	};

	enum CMP_FUNC
	{
		CMP_FUNC_GREATER,
		CMP_FUNC_GREATER_EQUAL,
		CMP_FUNC_EQUAL,
		CMP_FUNC_NOT_EQUAL,
		CMP_FUNC_LESS_EQUAL,
		CMP_FUNC_LESS,
		CMP_FUNC_ALWAYS,
		CMP_FUNC_NEVER
	};

	enum BLEND_FACTOR
	{
		BLEND_FACTOR_ZERO = 0,
		BLEND_FACTOR_ONE,
		BLEND_FACTOR_SRCCOLOR,
		BLEND_FACTOR_INVSRCCOLOR,
		BLEND_FACTOR_SRCALPHA,
		BLEND_FACTOR_INVSRCALPHA,
		BLEND_FACTOR_DESTCOLOR,
		BLEND_FACTOR_INVDESTCOLOR,
		BLEND_FACTOR_DESTALPHA,
		BLEND_FACTOR_INVDESTALPHA
	};

	enum BLEND_OP
	{
		BLEND_OP_ADD,
		BLEND_OP_SUBTRACT,
		BLEND_OP_REVSUBTRACT
	};

	enum STENCIL_OP
	{
		STENCIL_OP_KEEP,
		STENCIL_OP_ZERO,
		STENCIL_OP_REPLACE,
	};

	// texture ref
	enum TEX_ADDRESS
	{
		TEX_ADDRESS_REPEAT,
		TEX_ADDRESS_CLAMP,
		TEX_ADDRESS_MIRROR,
		TEX_ADDRESS_CLAMP_TO_ZERO,
		TEX_ADDRESS_CLAMP_TO_ONE,
	};

	enum TEX_FILTER
	{
		TEX_FILTER_NONE,
		TEX_FILTER_POINT,
		TEX_FILTER_LINEAR,
		TEX_FILTER_MIP_POINT,
		TEX_FILTER_MIP_LINEAR
	};

	struct SamplerState
	{
		TEX_ADDRESS     AddressU;
		TEX_ADDRESS     AddressV;

		TEX_FILTER      MinFilter;
		TEX_FILTER      MagFilter;

		SamplerState()
		{
			AddressU = TEX_ADDRESS_REPEAT;
			AddressV = TEX_ADDRESS_REPEAT;
			MinFilter = TEX_FILTER_LINEAR;
			MagFilter = TEX_FILTER_LINEAR;
		}
	};

	enum TEX_CLASS
	{
		TEX_CLASS_STATIC_RAW,
		TEX_CLASS_STATIC_COMPRESSED,
		TEX_CLASS_DYNAMIC,
		TEX_CLASS_RENDERTARGET,
	};

	enum PIXEL_FORMAT
	{
		PIXEL_FORMAT_RGBA8888,
		PIXEL_FORMAT_A8,
		PIXEL_FORMAT_PVRT,
		PIXEL_FORMAT_DEPTH_STENCIL,
		PIXEL_FORMAT_DEPTH_32,
		PIXEL_FORMAT_STENCIL_8
	};

	//// 基本数据结构

	struct PxTexDesc
	{
		size<PhU16>         size;
		PIXEL_FORMAT        format;
	};

	struct TexDesc
	{
		TEX_CLASS           eTexClass;
		PIXEL_FORMAT        ePixelFormat;
		PhSizeT				size;
	};

	struct ClearOP
	{
		bool				bClearColor;
		float				vClearColors[4];
		bool				bClearDepth;
		float               fDepthValue;
		bool		        bClearStencil;
		int16_t             nStencilValue;

		ClearOP()
		{
			bClearColor = true;
			vClearColors[0] = 1.0f;
			vClearColors[1] = 1.0f;
			vClearColors[2] = 1.0f;
			vClearColors[3] = 1.0f;
			bClearDepth = true;
			fDepthValue = 1.0f;
			bClearStencil = false;
			nStencilValue = 0;
		}

		void SetClearColor(float red, float green, float blue, float alpha)
		{
			this->vClearColors[0] = red;
			this->vClearColors[1] = green;
			this->vClearColors[2] = blue;
			this->vClearColors[3] = alpha;
		}
	};

	struct RenderState
	{
		bool	          depthWritable;
		bool              depthTestable;
		CMP_FUNC          depthFunc;

		bool              redChannelWritable;
		bool              greenChannelWritable;
		bool              blueChannelWritable;
		bool              alphaChannelWritable;

		CULL_MODE         cullMode;

		bool              scissorable;

		bool              blendable;
		BLEND_FACTOR      blendSrc;
		BLEND_FACTOR      blendDest;
		BLEND_OP          blendOpt;

		RenderState()
		{
			depthWritable = true;
			depthTestable = true;
			depthFunc = CMP_FUNC_LESS_EQUAL;

			redChannelWritable = true;
			greenChannelWritable = true;
			blueChannelWritable = true;
			alphaChannelWritable = true;
			blendOpt = BLEND_OP_ADD;

			blendDest = BLEND_FACTOR_ZERO;
			blendSrc = BLEND_FACTOR_ONE;

			cullMode = CULL_MODE_BACK;
			blendable = false;
			scissorable = false;
		}
	};
}