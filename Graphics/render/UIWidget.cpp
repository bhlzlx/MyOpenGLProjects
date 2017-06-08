#include "UIWidget.h"
#include "GUIRender.h"
#include <res/TexPool.h>

namespace ph
{
	void calc_rect_rel_param(const rect<float>& _rect1, const rect<float>& _rect2, rect<float>& _param)
	{
		_param.left = (_rect2.left - _rect1.left) / (_rect1.right - _rect1.left);
		_param.right = (_rect2.right - _rect1.left) / (_rect1.right - _rect1.left);
		_param.bottom = (_rect2.bottom - _rect1.bottom) / (_rect1.top - _rect1.bottom);
		_param.top = (_rect2.top - _rect1.bottom) / (_rect1.top - _rect1.bottom);
	}

	void calc_rect_from_rel_param(const rect<float>& _rect1, rect<float>& _param, rect<float>& _rect2)
	{
		_rect2.left = _rect1.left + (_rect1.right - _rect1.left) * _param.left;
		_rect2.right = _rect1.left + (_rect1.right - _rect1.left) * _param.right;
		_rect2.bottom = _rect1.bottom + (_rect1.top - _rect1.bottom) * _param.bottom;
		_rect2.top = _rect1.bottom + (_rect1.top - _rect1.bottom) * _param.top;
	}

	rect<float> UIWidget::__TheScissor;

	// nItem指UIRect的数量，是256个矩形
	void UIVertexArray::Init(size_t _nItem)
	{
		_nItem = (_nItem >> 2) << 2; // 4的倍数
		dynamicVBO = new DynamicVBO(_nItem, sizeof(UIRectVert));
		dynamicIBO = new dynamic_ibo(_nItem * 6 * sizeof(PhU32));
		vertexArray = VertexArray::New(dynamicVBO, dynamicIBO, &layouts[0]);
		std::vector<PhU32> Indices;
		for (size_t i = 0; i < _nItem; ++i)
		{
			Indices.push_back(i * 4 + 0);
			Indices.push_back(i * 4 + 3);
			Indices.push_back(i * 4 + 2);
			Indices.push_back(i * 4 + 0);
			Indices.push_back(i * 4 + 1);
			Indices.push_back(i * 4 + 3);
		}
		dynamicIBO->Write(0, _nItem * 6 * sizeof(PhU32), Indices.data());
	}
	void UIVertexArray::Bind()
	{
		vertexArray->Bind();
	}

	// 返回UIRect在VBO中的索引

	PhU32 UIVertexArray::AllocUIRect(const UIRectVert & _rect)
	{
		if (!stkFree.empty())
		{
			PhU32 ret = stkFree.top();
			dynamicVBO->Write(ret, &_rect);
			stkFree.pop();
			return ret;
		}
		size_t capacity = dynamicVBO->ItemCapacity();
		dynamicVBO->PushBack(&_rect);
		size_t new_capacity = dynamicVBO->ItemCapacity();
		if (new_capacity > capacity)
		{
			dynamicIBO->Resize(new_capacity * 6 * sizeof(PhU32));
			std::vector<PhU32> Indices;
			for (size_t i = capacity; i < new_capacity; ++i)
			{
				Indices.push_back(i * 4 + 0);
				Indices.push_back(i * 4 + 3);
				Indices.push_back(i * 4 + 2);
				Indices.push_back(i * 4 + 0);
				Indices.push_back(i * 4 + 1);
				Indices.push_back(i * 4 + 3);
			}
			dynamicIBO->Write(capacity * 6 * sizeof(PhU32), (new_capacity - capacity) * 6 * sizeof(PhU32), Indices.data());
		}
		return dynamicVBO->ItemCount() - 1;
	}

	void UIVertexArray::FreeUIRect(PhU32 _id)
	{
		stkFree.push(_id);
	}

	void UIVertexArray::Release()
	{
		vertexArray->Release();
		delete this;
	}

	void UIWidget::Clear()
	{
		/*GUIRender * render = GUIRender::GetInstance();
		for (auto& it : vecRenderTargets)
		{
			render->GetVertexPool()->FreeUIRect(it.vertId);
		}
		vecRenderTargets.clear();*/
	}

	void UIWidget::Begin()
	{
		GUIRender * render = GUIRender::GetInstance();
		for (auto& it : vecRenderTargets)
		{
			render->GetVertexPool()->FreeUIRect(it.vertId);
		}
		vecRenderTargets.clear();
		vecGRDRef.clear();
	}

	void UIWidget::End()
	{
		std::sort(vecRenderTargets.begin(), vecRenderTargets.end(), [](const UIWidget::UIRect& _rc1, const UIWidget::UIRect& _rc2) {
			return _rc1.layer < _rc2.layer;
		});
	}

	void UIWidget::Build( GDIRenderRect* _rect, PhU32 _color, float _gray, TexOGLRef _tex, TexOGLRef _mask, PhU32 _layer)
	{
		// 裁剪
		rect<float> clipped;
		rect<float> * pos = (rect<float> *)_rect;
		rect<float> * tc = pos + 1;
		rect<float> * mtc = tc + 1;
		bool clipRet = __TheScissor.clip(*pos, clipped);
		if (clipRet)
		{
			if (!memcmp(pos, &clipped, sizeof(rect<float>)) == 0)
			{
				ph::GDIRenderRect ClippedRC;
				rect<float> * clippedPos = (rect<float> *)&ClippedRC;
				rect<float> * clippedTc = clippedPos + 1;
				rect<float> * clippedMtc = clippedTc + 1;
				rect<float> param;
				calc_rect_rel_param(*pos, clipped, param);
				calc_rect_from_rel_param(*tc, param, *clippedTc);
				calc_rect_from_rel_param(*mtc, param, *clippedMtc);
				*clippedPos = clipped;
				*_rect = ClippedRC;
				//this->Build(ClippedRRC, nullptr, texture, _layer);
			}
			// else 不用变
		}
		else
		{
			return;
		}
		GUIRender * render = GUIRender::GetInstance();
		float PosYMin = (_rect->PosYMin / render->height * 2) - 1;
		float PosXMin = (_rect->PosXMin / render->width * 2) - 1;
		float PosYMax = (_rect->PosYMax / render->height * 2) - 1;
		float PosXMax = (_rect->PosXMax / render->width * 2) - 1;

		UIRectVert rv;
		rv.LeftBottom.xy.x = PosXMin;
		rv.LeftBottom.xy.y = PosYMin;
		rv.LeftBottom.tc.x = _rect->TcXMin;
		rv.LeftBottom.tc.y = _rect->TcYMin;
		rv.LeftBottom.mtc.x = _rect->MtcXMin;
		rv.LeftBottom.mtc.y = _rect->MtcYMin;

		rv.RightBottom.xy.x = PosXMax;
		rv.RightBottom.xy.y = PosYMin;
		rv.RightBottom.tc.x = _rect->TcXMax;
		rv.RightBottom.tc.y = _rect->TcYMin;
		rv.RightBottom.mtc.x = _rect->MtcXMax;
		rv.RightBottom.mtc.y = _rect->MtcYMin;

		rv.LeftTop.xy.x = PosXMin;
		rv.LeftTop.xy.y = PosYMax;
		rv.LeftTop.tc.x = _rect->TcXMin;
		rv.LeftTop.tc.y = _rect->TcYMax;
		rv.LeftTop.mtc.x = _rect->MtcXMin;
		rv.LeftTop.mtc.y = _rect->MtcYMax;

		rv.RightTop.xy.x = PosXMax;
		rv.RightTop.xy.y = PosYMax;
		rv.RightTop.tc.x = _rect->TcXMax;
		rv.RightTop.tc.y = _rect->TcYMax;
		rv.RightTop.mtc.x = _rect->MtcXMax;
		rv.RightTop.mtc.y = _rect->MtcYMax;

		PhU32 id = GUIRender::GetInstance()->GetVertexPool()->AllocUIRect(rv);

		UIRect rc;
		rc.layer = _layer;
		if (_mask == nullptr)
			rc.mask = TexPool::GetWhite();
		else
			rc.mask = _mask;
		if (_tex == nullptr)
			rc.texture = TexPool::GetWhite();
		else
			rc.texture = _tex;
		rc.vertId = id;

		rc.color.x = (_color >> 24) / 255.0f;
		rc.color.y = (_color >> 16 & 0xff) / 255.0f;
		rc.color.z = (_color >> 8 & 0xff) / 255.0f;
		rc.color.w = (_color & 0xff) / 255.0f;
		rc.gray = _gray;

		this->vecRenderTargets.push_back(rc);
	}

	void UIWidget::Build( GDIRenderRectExt & _rect, TexOGLRef _tex, TexOGLRef _mask, PhU32 _layer)
	{
		// 裁剪
		rect<float> clipped;
		rect<float> * pos = (rect<float> *)&_rect;
		rect<float> * tc = pos + 1;
		rect<float> * mtc = tc + 1;
		bool clipRet = __TheScissor.clip(*pos, clipped);
		if (clipRet)
		{
			if (!memcmp(pos, &clipped, sizeof(rect<float>)) == 0)
			{
				ph::GDIRenderRectExt ClippedRRC;
				ClippedRRC.Color = _rect.Color;
				ClippedRRC.Gray = _rect.Gray;
				rect<float> * clippedPos = (rect<float> *)&ClippedRRC;
				rect<float> * clippedTc = clippedPos + 1;
				rect<float> * clippedMtc = clippedTc + 1;
				rect<float> param;
				calc_rect_rel_param(*pos, clipped, param);
				calc_rect_from_rel_param(*tc, param, *clippedTc);
				calc_rect_from_rel_param(*mtc, param, *clippedMtc);
				*clippedPos = clipped;
				_rect = ClippedRRC;
				//this->Build(ClippedRRC, nullptr, texture, _layer);
			}
			// else 不用变
		}
		else
		{
			return;
		}

		GUIRender * render = GUIRender::GetInstance();
		float PosYMin = (_rect.PosYMin / render->height * 2) - 1;
		float PosXMin = (_rect.PosXMin / render->width * 2) - 1;
		float PosYMax = (_rect.PosYMax / render->height * 2) - 1;
		float PosXMax = (_rect.PosXMax / render->width * 2) - 1;

		UIRectVert rv;
		rv.LeftBottom.xy.x = PosXMin;
		rv.LeftBottom.xy.y = PosYMin;
		rv.LeftBottom.tc.x = _rect.TcXMin;
		rv.LeftBottom.tc.y = _rect.TcYMin;
		rv.LeftBottom.mtc.x = _rect.MtcXMin;
		rv.LeftBottom.mtc.y = _rect.MtcYMin;

		rv.RightBottom.xy.x = PosXMax;
		rv.RightBottom.xy.y = PosYMin;
		rv.RightBottom.tc.x = _rect.TcXMax;
		rv.RightBottom.tc.y = _rect.TcYMin;
		rv.RightBottom.mtc.x = _rect.MtcXMax;
		rv.RightBottom.mtc.y = _rect.MtcYMin;

		rv.LeftTop.xy.x = PosXMin;
		rv.LeftTop.xy.y = PosYMax;
		rv.LeftTop.tc.x = _rect.TcXMin;
		rv.LeftTop.tc.y = _rect.TcYMax;
		rv.LeftTop.mtc.x = _rect.MtcXMin;
		rv.LeftTop.mtc.y = _rect.MtcYMax;

		rv.RightTop.xy.x = PosXMax;
		rv.RightTop.xy.y = PosYMax;
		rv.RightTop.tc.x = _rect.TcXMax;
		rv.RightTop.tc.y = _rect.TcYMax;
		rv.RightTop.mtc.x = _rect.MtcXMax;
		rv.RightTop.mtc.y = _rect.MtcYMax;

		PhU32 id = GUIRender::GetInstance()->GetVertexPool()->AllocUIRect(rv);

		UIRect rc;
		rc.layer = _layer;
		if (_mask == nullptr)
			rc.mask = TexPool::GetWhite();
		else
			rc.mask = _mask;
		if (_tex == nullptr )
			rc.texture = TexPool::GetWhite();
		else
			rc.texture = _tex;
		rc.vertId = id;

		rc.color.x = (_rect.Color >> 24) / 255.0f;
		rc.color.y = (_rect.Color >> 16 & 0xff ) / 255.0f;
		rc.color.z = (_rect.Color >> 8 & 0xff) / 255.0f;
		rc.color.w = (_rect.Color & 0xff ) / 255.0f;
		rc.gray = _rect.Gray;

		this->vecRenderTargets.push_back(rc);
	}

	void UIWidget::Build(const UIText& _text, PhU32 _layer)
	{
		float baseX = _text.x;
		ph::GDIRenderRectExt rrc;

		rrc.Color = _text.color;
		rrc.Gray = 0.0f;

		Glypher* glypher = nullptr;

		GUIRender * render = GUIRender::GetInstance();
		if (render)
		{
			glypher = &render->glypher;
		}
		if (!glypher)
			return;

		size_t i = 0;
		while (i < _text.length )
		{
			if (_text.text[i] < 255)
			{
				if (!isprint(_text.text[i]))
				{
					++i;
					continue;
				}
				else if (_text.text[i] == u' ')
				{
					baseX += glypher->FaceHeight(_text.size) / 2;
					++i;
					continue;
				}
			}
			ph::GD gd;
			gd.bound = _text.size;
			gd.unicode = _text.text[i];
			ph::GRDRef grd = glypher->Get(gd);

			rrc.PosXMin = baseX +(float)grd->penx;
			rrc.PosXMax = (float)grd->bmpWidth + rrc.PosXMin;
			rrc.PosYMin = (float)grd->peny + _text.y;
			rrc.PosYMax = (float)grd->bmpHeight + rrc.PosYMin;

			rrc.MtcXMin = grd->tcxMin;
			rrc.MtcXMax = grd->tcxMax;
			rrc.MtcYMin = grd->tcyMin;
			rrc.MtcYMax = grd->tcyMax;

			rrc.TcXMin = 0.25f;
			rrc.TcXMax = 0.75f;
			rrc.TcYMin = 0.25f;
			rrc.TcYMax = 0.75f;

			vecGRDRef.push_back( grd );
			TexOGLRef texture = glypher->GetCanvasTexRef(grd->canvasId);
			this->Build(rrc, nullptr, texture, _layer);
			baseX = rrc.PosXMax;
			baseX += _text.charGap;
			++i;
		}
	}

	bool UIWidget::GetStringRect(const char16_t * _str, size_t _size, size_t _nChar, float _gap, float & _width, float & _height)
	{
		_width = 0.0f;

		Glypher* glypher = nullptr;

		GUIRender * render = GUIRender::GetInstance();
		if (render)
		{
			glypher = &render->glypher;
		}
		if (!glypher)
			return false;

		float baseX = 0.0f;
		ph::GD gd;
		gd.bound = (uint16_t)_size;

		size_t i = 0;
		while (i < _nChar - 1)
		{
			if (_str[i] < 255)
			{
				if (!isprint(_str[i]))
				{
					++i;
					continue;
				}
			}
			gd.unicode = _str[i];
			ph::GRDRef grd = glypher->Get(gd);
			_width += grd->width;
			_width += _gap;
			_height = grd->height > _height ? grd->height : _height;
			++i;
		}
		if (!( _str[i] < 255 && !isprint(_str[i])))
		{
			ph::GRDRef grd;
			gd.unicode = _str[i];
			grd = glypher->Get(gd);
			_height = grd->height > _height ? grd->height : _height;
			_width += grd->width;
		}
		return true;
	}

	void UIWidget::DefScissor(float _left, float _right, float _bottom, float _top)
	{
		__TheScissor.set(_left, _right, _bottom, _top);
	}
}