#pragma once
#include <graphics/BufferOGL.h>
#include <graphics/TexOGL.h>
#include <stack>
#include <memory>
#include <algorithm>
#include <render/glypher.h>

namespace ph
{

	void calc_rect_rel_param(const rect<float>& _rect1, const rect<float>& _rect2, rect<float>& _param);
	void calc_rect_from_rel_param(const rect<float>& _rect1, rect<float>& _param, rect<float>& _rect2);

	struct UIVert
	{
		vec2	xy;
		vec2	tc;
		vec2	mtc;
	};

	struct UIRectVert
	{
		UIVert LeftBottom;
		UIVert RightBottom;
		UIVert LeftTop;
		UIVert RightTop;
	};

	struct GDIRenderRect
	{
		float PosXMin;
		float PosXMax;
		float PosYMin;
		float PosYMax;

		float TcXMin;
		float TcXMax;
		float TcYMin;
		float TcYMax;

		float MtcXMin;
		float MtcXMax;
		float MtcYMin;
		float MtcYMax;
	};

	struct GDIRenderRectExt
	{
		float PosXMin;
		float PosXMax;
		float PosYMin;
		float PosYMax;

		float TcXMin;
		float TcXMax;
		float TcYMin;
		float TcYMax;

		float MtcXMin;
		float MtcXMax;
		float MtcYMin;
		float MtcYMax;

		PhU32 Color;
		float Gray;
	};

	static_assert(sizeof(UIVert) == sizeof(float) * 6, "UIVert <==> VBO size does mismatch!");
	static_assert(sizeof(UIRectVert) == sizeof(float) * 24, "UIRectVert <==> VBO size does mismatch!");

	static VertexArray::Layout  layouts[] = {
		{ 0, 2, GL_FLOAT, sizeof(float) * 6, 0 },
		{ 1, 2, GL_FLOAT, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 2) },
		{ 2, 2, GL_FLOAT, sizeof(float) * 6, (GLvoid*)(sizeof(float) * 4) },
		{ 0, 0, 0, 0, 0 }
	};

	struct UIVertexArray
	{
	private:
		DynamicVBORef								dynamicVBO;
		DynamicIBORef								dynamicIBO;
		VertexArrayRef								vertexArray;

		std::stack<PhU32> stkFree;
	public:
		// nItem指UIRect的数量，是256个矩形
		void Init(size_t _nItem = 256);

		void Bind();

		// 返回UIRect在VBO中的索引
		PhU32 AllocUIRect(const UIRectVert& _rect);

		void FreeUIRect(PhU32 _id);

		void Release();
	};

	struct UIText
	{
		char16_t*		text;
		PhU16			length;
		PhU8			size;
		PhU32			color;
		float			x;
		float			y;
		float			charGap;
		// 旋转变换？
		float			angle;
		vec2			center;
	};

	struct UIVertex
	{
		vec2	pos;
		vec2	tc_base;
		vec2	tc_mask;
		PhU32	col;
		float	gray;
	};

	class GUIRender;
	struct UIWidget
	{
	private:
		friend class GUIRender;
		struct UIRect
		{
			TexOGLRef					texture;
			TexOGLRef					mask;
			PhU32						vertId;
			PhU32						layer;
			vec4						color;
			float						gray;
		};
		std::vector< UIRect >	        vecRenderTargets;
		std::vector< GRDRef>			vecGRDRef;
		static rect<float>				__TheScissor;
	public:
		void Clear();

		void Begin();

		void End();

		void Build(GDIRenderRect* _rect, PhU32 _color, float _gray, TexOGLRef _tex, TexOGLRef _mask, PhU32 _layer);
		void Build(GDIRenderRectExt& _rect, TexOGLRef _tex, TexOGLRef _mask, PhU32 _layer);
		void Build(const UIText& _text, PhU32 _layer);

		static bool GetStringRect(const char16_t * _str, size_t _size, size_t _nChar, float _gap, float& _width, float& _height);
		static void	DefScissor(float _left, float _right, float _bottom, float _top);
	};
}
